/*
    Copyright (C) 2010 Sébastien Escudier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QKeyEvent>
#include <QDebug>
#include <QTimer>

#include "GamePlay.h"
#include "GameField.h"
#include "NetClient.h"
#include "NetServer.h"
#include "constant.h"
#include "Map.h"
#include "Settings.h"

GamePlay::GamePlay(QMainWindow *mainw, Settings *set)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(slotMoveTimer()));
    leftK = rightK = upK = downK = false;
    gameField = new GameField(mainw, BLOCK_SIZE);
    gameField->getEventFilter(this);
    //MAP_SIZE
    client = new NetClient;
    connect(client, SIGNAL(sigConnected()), this, SLOT(slotClientConnected()));
    connect(client, SIGNAL(sigConnectionError()), this, SLOT(slotClientConnectError()));
    settings = set;
}

void GamePlay::launch()
{
    /**
     * If we act as a server we must create the map
     * If we act as a client we must wait to receive the map from the server
     */
    if( settings->getServer() )
    {
        //we are the server
        gameField->createRandomMap(MAP_SIZE,MAP_SIZE);
        const Map *m = gameField->getMap();
        server = new NetServer(m,settings->getServerPort());
        server->start();
        //We start the game as soon as a player is connecter to the server (ourselves actualy)
        //others players can join later (but they may miss movement me can make before they join
        //the way the game is launched will be changed later.
        connect(server,SIGNAL(newPlayer()),this,SLOT(slotStart()),Qt::QueuedConnection);
        gameField->createGraphics();
    }
    else
    {
        server = NULL;
        //we will need the map before we can start
        connect(client,SIGNAL(mapReceived(const Map*)),this,SLOT(mapReceived(const Map*)));
    }
    client->connectToServer(settings->getServerAddress(), settings->getServerPort());
    connect(client,SIGNAL(moveReceived(qint16,qint16,qint16)),this,SLOT(moveReceived(qint16,qint16,qint16)));
}

void GamePlay::mapReceived(const Map *map)
{
    //todo doesnt work yet
    gameField->setMap(map);
    gameField->createGraphics();
}

void GamePlay::slotStart()
{
    server->assignNumberToPlayers();
}

void GamePlay::move(int direction)
{
    client->sendMove(direction);
}

void GamePlay::moveReceived(qint16 plId, qint16 x, qint16 y)
{
    gameField->movePlayer(plId, x, y);
}

/**
 *      1
 *      |
 *  0 <- -> 2
 *      |
 *      3
 */
void GamePlay::slotMoveTimer()
{
    int direction;
    if(leftK)
        direction = 0;
    else if(rightK)
        direction = 2;
    else if(upK)
        direction = 1;
    else if(downK)
        direction = 3;
    client->sendMove(direction);
}

bool GamePlay::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *c = ( QKeyEvent * )event;
        if(c->key() == Qt::Key_Escape)
        {
            qDebug("GamePlay escape");
        }
        else if(c->key() == Qt::Key_Space)
        {
            qDebug("space");
            //dropBomb(0);
        }
    }
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *c = ( QKeyEvent * )event;
        if(c->isAutoRepeat())
            return true;
        bool press = event->type() == QEvent::KeyPress;
        if(c->key() == Qt::Key_Left)
            leftK = press;
        else if(c->key() == Qt::Key_Up)
            upK = press;
        else if(c->key() == Qt::Key_Right)
            rightK = press;
        else if(c->key() == Qt::Key_Down)
            downK = press;
        if(press && !timer->isActive())
        {
            timer->start(MOVE_TICK_INTERVAL);
            slotMoveTimer();
        }
        if(!press && !leftK && !upK && !rightK && !downK && timer->isActive())
        {
            timer->stop();
        }
        return true;
    }
    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
void GamePlay::slotClientConnected()
{
    emit connectedToServer();
}

void GamePlay::slotClientConnectError()
{
    emit connectionError();
}

GamePlay::~GamePlay()
{
    delete gameField;
    if(server)
    {
        server->quit();
        server->wait();
        delete server;
    }
    delete client;
    delete timer;
}

