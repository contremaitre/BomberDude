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

#include "GamePlay.h"
#include "GameField.h"
#include "NetClient.h"
#include "NetServer.h"
#include "constant.h"

#include <QKeyEvent>
#include <QDebug>

GamePlay::GamePlay()
{

    gameField = new GameField(MAP_SIZE,MAP_SIZE,BLOCK_SIZE);
    gameField->getEventFilter(this);
    const Map *m = gameField->getMap();

    client = new NetClient;
    server = new NetServer(m);

    server->start();
    client->connectToServer("localhost", SERVER_PORT);
    //We start the game as soon as a player is connecter to the server (ourselves actualy)
    //the way the game is launched will be changed later.
    connect(server,SIGNAL(newPlayer()),this,SLOT(slotStart()),Qt::QueuedConnection);
    connect(client,SIGNAL(moveReceived(int,int)),this,SLOT(moveReceived(int,int)));
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

