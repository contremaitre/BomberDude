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

GamePlay::GamePlay(Settings *set, QGraphicsView *mapGraphicPreview, QString player1name)
                    : player1name(player1name)
{
    //qDebug() << "new Gameplay";
    connect(&timer, SIGNAL(timeout()),this,SLOT(slotMoveTimer()));
    connect(&timerPing, SIGNAL(timeout()),this,SLOT(slotPingTimer()));
    timerPing.start(2000); // Ping every 2s

    leftK1 = rightK1 = upK1 = downK1 = false;
    leftK2 = rightK2 = upK2 = downK2 = false;

    /* Load pixmaps */
    QFlame::loadPixs();
    QBomb::loadPixs();
    QPlayer::loadPixs();

    gameArena = new GameArena(BLOCK_SIZE);
    gameArenaPreview = new GameArena(BLOCK_SIZE/2);  //todo : create a light class for the preview
    gameArenaPreview->setGraphicView(mapGraphicPreview);
    gameArena->getEventFilter(this);
    connect(gameArena, SIGNAL(sigTimeUpdated(int)), this, SLOT(slotTimeUpdated(int)));
    connect(gameArena, SIGNAL(sigNewPlayerGraphic(qint8,const QPixmap &)), this, SLOT(slotNewPlayerGraphic(qint8,const QPixmap &)));

    client = new NetClient;
    clientPlayer2 = NULL;
    connect(client,SIGNAL(mapReceived(MapClient*)),this,SLOT(mapReceived(MapClient*)));
    connect(client,SIGNAL(mapPreviewReceived(MapClient*)),this,SLOT(mapPreviewReceived(MapClient*)));
    connect(client,SIGNAL(sigMapRandom(bool)),this,SLOT(slotMapRandom(bool)));
    connect(client, SIGNAL(sigMapWinner(qint8,bool)), gameArena, SLOT(slotMapWinner(qint8,bool)));
    connect(client, SIGNAL(sigMapWinner(qint8,bool)), this, SLOT(slotEndRound(qint8,bool)));

    connect(client, SIGNAL(sigConnected()), this, SLOT(slotConnectedToServer()));

    settings = set;

    memset(&player1Keys, 0, sizeof(player1Keys));
    memset(&player2Keys, 0, sizeof(player2Keys));
}

void GamePlay::addPlayer(const QString &name)
{
    player2name = name;
    delete clientPlayer2;
    clientPlayer2 = new NetClient;
    connect( clientPlayer2, SIGNAL(sigConnected()), this, SLOT(slotConnectedToServer()));
    connect( clientPlayer2, SIGNAL(sigConnectionError()), this, SLOT(slotPlayer2Disconnected()), Qt::QueuedConnection);
    connect( clientPlayer2, SIGNAL(sigNetClientEnd()), this, SLOT(slotPlayer2Disconnected()));

    if( settings->isServer() )
        clientPlayer2->connectToServer("127.0.0.1", settings->getServerPort());
    else
        clientPlayer2->connectToServer(settings->getServerAddress(), settings->getServerPort());
}

void GamePlay::cliConnect(const QString &pass)
{
    client->setAdminPasswd(pass);
    //we will need the map before we can start
    if( settings->isServer() )
        client->connectToServer("127.0.0.1", settings->getServerPort());
    else
        client->connectToServer(settings->getServerAddress(), settings->getServerPort());
    player1Keys = settings->getPlayer1Keys();
    player2Keys = settings->getPlayer2Keys();
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

void GamePlay::slotConnectedToServer()
{
    qDebug("GamePlay::slotConnected");
    if(!clientPlayer2)
        client->sendPlayerData(player1name);
    else
        clientPlayer2->sendPlayerData(player2name);
}

void GamePlay::slotPlayer2Disconnected()
{
    delete clientPlayer2;
    clientPlayer2 = NULL;
}

void GamePlay::slotEndRound(qint8, bool end)
{
    if(!end)
        QTimer::singleShot(1000*5, this, SLOT(slotLoadInterGame()));
}

void GamePlay::slotLoadInterGame()
{
    emit sigLoadInterGame();
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
    if(leftK1)
    {
        if(upK1)
            direction = 1;
        else if(downK1)
            direction = 7;
        else
            direction = 0;
    }
    else if(rightK1)
    {
        if(upK1)
            direction = 3;
        else if(downK1)
            direction = 5;
        else
            direction = 4;
    }
    else if(upK1)
    {
        direction = 2;
    }
    else if(downK1)
    {
        direction = 6;
    }
    //qDebug() << "GamePlay direction=" << direction;
    if( direction != -1 )
        client->sendMove(direction);

    if( !clientPlayer2 )
        return;

    direction = -1;
    if(leftK2)
    {
        if(upK2)
            direction = 1;
        else if(downK2)
            direction = 7;
        else
            direction = 0;
    }
    else if(rightK2)
    {
        if(upK2)
            direction = 3;
        else if(downK2)
            direction = 5;
        else
            direction = 4;
    }
    else if(upK2)
    {
        direction = 2;
    }
    else if(downK2)
    {
        direction = 6;
    }
    if( direction != -1 )
        clientPlayer2->sendMove(direction);
}

void GamePlay::slotPingTimer()
{
    client->sendPing();
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
        else if(c->key() == player1Keys.drop)
        {
            client->sendBomb();
            return true;
        }
        else if(c->key() == player1Keys.opt)
        {
            client->sendOptKey();
        }
        else if( clientPlayer2 )
        {
            if(c->key() == player2Keys.drop)
            {
                clientPlayer2->sendBomb();
                return true;
            }
            else if(c->key() == player2Keys.opt)
            {
                clientPlayer2->sendOptKey();
            }
        }
    }
    if(event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *c = ( QKeyEvent * )event;
        if(c->isAutoRepeat())
            return true;
        bool press = event->type() == QEvent::KeyPress;
        if(c->key() == player1Keys.left)
            leftK1 = press;
        else if(c->key() == player1Keys.up)
            upK1 = press;
        else if(c->key() == player1Keys.right)
            rightK1 = press;
        else if(c->key() == player1Keys.down)
            downK1 = press;
        else if(c->key() == player2Keys.left)
            leftK2 = press;
        else if(c->key() == player2Keys.up)
            upK2 = press;
        else if(c->key() == player2Keys.right)
            rightK2 = press;
        else if(c->key() == player2Keys.down)
            downK2 = press;
        else
            return QObject::eventFilter(obj, event);
        if(press && !timer.isActive())
        {
            timer.start(HEARTBEAT);
            slotMoveTimer();
        }
        if(        !press
                && !leftK1 && !upK1 && !rightK1 && !downK1
                && !leftK2 && !upK2 && !rightK2 && !downK2
                && timer.isActive() )
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
    //qDebug() << "Gameplay destructor";
    delete gameArena;
    delete gameArenaPreview;
    delete client;
    delete clientPlayer2;

    /* Unload pixmaps */
    QFlame::unloadPixs();
    QBomb::unloadPixs();
    QPlayer::unloadPixs();
}
