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

#include "GamePlay.h"
#include "GameField.h"
#include "NetClient.h"
#include "NetServer.h"
#include "constant.h"
#include "Map.h"

GamePlay::GamePlay(QMainWindow *mainw)
{

    gameField = new GameField(mainw, BLOCK_SIZE);
    gameField->getEventFilter(this);
    //MAP_SIZE
    client = new NetClient;

    /**
     * If we act as a server we must create the map
     * If we act as a client we must wait to receive the map from the server
     */
    if(QString(SERVER_ADRESS) == QString("localhost"))
    {
        //we are the server
        gameField->createRandomMap(MAP_SIZE,MAP_SIZE);
        const Map *m = gameField->getMap();
        server = new NetServer(m);
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
    client->connectToServer(SERVER_ADRESS, SERVER_PORT);
    connect(client,SIGNAL(moveReceived(int,int)),this,SLOT(moveReceived(int,int)));
    
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

void GamePlay::moveReceived(int plId, int position)
{
    gameField->movePlayer(plId,position);
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
        else if(c->key() == Qt::Key_Left)
        {
            move(0);
        }
        else if(c->key() == Qt::Key_Up)
        {
            move(1);
        }
        else if(c->key() == Qt::Key_Right)
        {
            move(2);
        }
        else if(c->key() == Qt::Key_Down)
        {
            move(3);
        }
        return true;
    }
    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
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
}

