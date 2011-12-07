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
#include "constant.h"

GamePlay::GamePlay(Settings *set, QGraphicsView *mapGraphicPreview)
{
    connect(&timer, SIGNAL(timeout()),this,SLOT(slotMoveTimer()));
    connect(&timerPing, SIGNAL(timeout()),this,SLOT(slotPingTimer()));
    timerPing.start(2000); // Ping every 2s

    leftK = rightK = upK = downK = false;
    gameArena = new GameArena(BLOCK_SIZE);
    gameArenaPreview = new GameArena(BLOCK_SIZE/2);  //todo : create a light class for the preview
    gameArenaPreview->setGraphicView(mapGraphicPreview);
    gameArena->getEventFilter(this);
    connect(gameArena, SIGNAL(sigTimeUpdated(int)), this, SLOT(slotTimeUpdated(int)));
    connect(gameArena, SIGNAL(sigNewPlayerGraphic(qint8,const QPixmap &)), this, SLOT(slotNewPlayerGraphic(qint8,const QPixmap &)));

    //MAP_SIZE
    client = new NetClient;
    connect(client,SIGNAL(mapReceived(MapClient*)),this,SLOT(mapReceived(MapClient*)));
    connect(client,SIGNAL(mapPreviewReceived(MapClient*)),this,SLOT(mapPreviewReceived(MapClient*)));
    connect(client,SIGNAL(sigMapRandom(bool)),this,SLOT(slotMapRandom(bool)));
    connect(client, SIGNAL(sigMapWinner(qint8)), gameArena, SLOT(slotMapWinner(qint8)));
    settings = set;
}

void GamePlay::cliConnect(const QString &pass)
{
    client->setAdminPasswd(pass);
    //we will need the map before we can start
    if( settings->isServer() )
        client->connectToServer("127.0.0.1", settings->getServerPort());
    else
        client->connectToServer(settings->getServerAddress(), settings->getServerPort());
}

void GamePlay::mapReceived(MapClient *map)
{
    //qDebug() << "map received";
    gameArena->setMap(map);
}

void GamePlay::gameStarted(QGraphicsView *graphicView)
{
    //qDebug() << "map received";
    gameArena->setGraphicView(graphicView);
}

void GamePlay::slotMapRandom(bool rand)
{
    if(rand)
        gameArenaPreview->clear();
}

void GamePlay::mapPreviewReceived(MapClient *map)
{
    gameArenaPreview->setMap(map);
}

void GamePlay::slotTimeUpdated(int timeInSeconds) {
    emit sigTimeUpdated(timeInSeconds);
}

void GamePlay::slotNewPlayerGraphic(qint8 pl, const QPixmap &pix) {
    emit sigNewPlayerGraphic(pl,pix);
}

void GamePlay::move(int direction)
{
    client->sendMove(direction);
}

//void GamePlay::moveReceived(qint16 plId, qint16 x, qint16 y)
//{
//    gameArena->movePlayer(plId, x, y);
//}

/**
 *   1  2  3
 *    \ | /
 *  0 <- -> 4
 *    / | \
 *   7  6  5
 */
void GamePlay::slotMoveTimer()
{
    int direction = -1;
    if(leftK)
    {
        if(upK)
            direction = 1;
        else if(downK)
            direction = 7;
        else
            direction = 0;
    }
    else if(rightK)
    {
        if(upK)
            direction = 3;
        else if(downK)
            direction = 5;
        else
            direction = 4;
    }
    else if(upK)
    {
        direction = 2;
    }
    else if(downK)
    {
        direction = 6;
    }
    //qDebug() << "GamePlay direction=" << direction;
    client->sendMove(direction);
}

void GamePlay::slotPingTimer()
{
    client->sendPing();
}

void GamePlay::dropBomb()
{
    client->sendBomb();
}

bool GamePlay::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *c = ( QKeyEvent * )event;
        if(c->key() == Qt::Key_Escape)
        {
            qDebug("GamePlay escape");
            emit quitGame();
            return true;
        }
        else if(c->key() == Qt::Key_Space)
        {
            dropBomb();
            return true;
        }
        else if(c->key() == Qt::Key_Control)
        {
            client->sendOptKey();
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
        else
            return QObject::eventFilter(obj, event);
        if(press && !timer.isActive())
        {
            timer.start(HEARTBEAT);
            slotMoveTimer();
        }
        if(!press && !leftK && !upK && !rightK && !downK && timer.isActive())
        {
            timer.stop();
        }
        return true;
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

NetClient *GamePlay::getNetClient()
{
    return client;
}

GamePlay::~GamePlay()
{
    delete gameArena;
    delete gameArenaPreview;
    delete client;
}
