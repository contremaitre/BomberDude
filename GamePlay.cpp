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

GamePlay::GamePlay()
{

    client = new NetClient;
    server = new NetServer;

    gameField = new GameField(MAP_SIZE,MAP_SIZE,BLOCK_SIZE);
    gameField->getEventFilter(this);

    server->start();
    client->connectToServer("localhost", SERVER_PORT);

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

/**
 *      1
 *      |
 *  0 <- -> 2
 *      |
 *      3
 */
bool GamePlay::move(int id, int direction)
{
    int x,y;
    bool ok = false;
/*
    caseList.getPlayerPosition(id,x,y);
    qDebug("player %d, direction %d, pos (%d,%d)",id,direction,x,y);
    switch(direction)
    {
        case 0:
            ok = caseList.movePlayer(id,x-1,y);
            break;

        case 1:
            ok = caseList.movePlayer(id,x,y-1);        
            break;

        case 2:
            ok = caseList.movePlayer(id,x+1,y);        
            break;
        case 3:
            ok = caseList.movePlayer(id,x,y+1);        
            break;
    }
*/
    qDebug("move %s",ok?"ok":"failed");
    return ok;
}

bool GamePlay::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyRelease)
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
            move(0,0);
        }
        else if(c->key() == Qt::Key_Up)
        {
            move(0,1);
        }
        else if(c->key() == Qt::Key_Right)
        {
            move(0,2);
        }
        else if(c->key() == Qt::Key_Down)
        {
            move(0,3);
        }
        return true;
    }
    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

