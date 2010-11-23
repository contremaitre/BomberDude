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

#include <QDebug>
#include <QDataStream>

#include "startUi.h"
#include "Flame.h"
#include "GameArena.h"

GameArena::GameArena(QMainWindow * mainw, int s) :
	map(0),
    timeInSeconds(-999)
{
	width = height = 0;
	squareSize = s;
	squaresItem = NULL;
	playersItem = NULL;
    scene = new QGraphicsScene;
    mainWindow = mainw;
    view = NULL;
	loadPixMaps();
}

void GameArena::loadPixMaps()
{
	pixmaps.init(squareSize, squareSize);
}

void GameArena::init()
{
	width = map->getWidth();
	height = map->getHeight();
	delete[] squaresItem;
	delete[] playersItem; //todo we may have *playersItem leaks
	squaresItem = new QGraphicsSquareItem*[width * height];
	playersItem = new QGraphicsSquareItem*[map->getMaxNbPlayers()];
	for(int i = 0; i < width; i++)
	{
		for(int j = 0; j < height; j++)
		{
			initCase(i,j);
			getCase(i,j)->setItem(pixmaps.getPixmap(map->getType(i,j)));
		}
	}
	qint16 x,y;
	for(int i = 0; i < map->getMaxNbPlayers(); i++)
	{
		map->getPlayerPosition(i,x,y);
		playersItem[i] = new QGraphicsSquareItem(x-squareSize/2,y-squareSize/2,squareSize);
		playersItem[i]->setItem(pixmaps.getPixmap(i));
	}
}

void GameArena::createGraphics()
{
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            QGraphicsSquareItem *m_case = getCase(i,j);
            scene->addItem(m_case->getItem());
        }
    }
    for(int i = 0 ; i < getNbPlayers(); i++)
    {
            QGraphicsSquareItem *m_case = getPlayer(i);
            scene->addItem(m_case->getItem());
    }
    view = new QGraphicsView(mainWindow);
    int size = squareSize * (width+1);
    mainWindow->setMinimumSize(size,size);
    view->setMinimumSize(size,size);
    view->setScene(scene);
    view->show();
    view->setFocus();
}

void GameArena::getEventFilter(QObject *obj)
{
    scene->installEventFilter(obj);
}

void GameArena::setMap(MapClient *map)
{
	this->map = map;
	init();
    connect(map, SIGNAL(sigBlockChanged(int)), this, SLOT(blockChanged(int)));
    connect(map, SIGNAL(sigBlockChanged(int,int)), this, SLOT(blockChanged(int,int)));
    connect(map, SIGNAL(sigHeartbeatUpdated(qint32)), this, SLOT(slotHearbeatUpdated(qint32)));
}

void GameArena::movePlayer(int player, int x, int y)
{
    Q_ASSERT(playersItem[player] != NULL); //Player really exists
	map->setPlayerPosition(player, x, y);
	playersItem[player]->setPos(x-squareSize/2,y-squareSize/2,squareSize);
}

void GameArena::addBomb(int player, int squareX, int squareY, int bombId)
{
	map->addBomb(player, squareX, squareY, bombId);
}

void GameArena::addFlame(Flame* flame)
{
	map->addFlame(flame);
}


void GameArena::removeBomb(qint16 bombId)
{
	map->removeBomb(bombId);
}

void GameArena::removeFlame(int flameId)
{
	map->removeFlame(flameId);
}

int GameArena::getCaseSize() const
{
	return squareSize;
}

void GameArena::initCase(int i, int j)
{
	int x_a = i*squareSize;
	int y_a = j*squareSize;
	squaresItem[j*width+i] = new QGraphicsSquareItem(x_a,y_a,squareSize);
}

void GameArena::blockChanged(int pos)
{
	QGraphicsSquareItem* tempItem = getCase(pos);
	//qDebug() << "Wall: " << tempItem->getItem()->pixmap().cacheKey() << ", new value: " << pixmaps.getPixmap(map->getType(pos)).cacheKey();
	tempItem->setItem(pixmaps.getPixmap(map->getType(pos)));
}

void GameArena::updateMap(QByteArray& updateBlock) {
	// FIXME this method should not be called before the map has been setup,
	// i.e. the calling method which is a slot should not be connected before
	// I'll leave that as an exercise to whoever fixes a bit the workflow of
	// the client :p
	if(map == 0)
		return;

	QDataStream updateIn(updateBlock);

	qint32 heartBeat;
	updateIn >> heartBeat;
	map->setHeartBeat(heartBeat);
	if(heartBeat % 100 == 0)
	    qDebug() << "Received heartbeat " << heartBeat;

	QList<qint16> cleanList;
	updateIn >> cleanList;
	foreach(qint16 flameId, cleanList)
		removeFlame(flameId);

	qint8 playerListSize;
	updateIn >> playerListSize;
//	qDebug() << playerListSize << " players received";
	for(qint8 i = 0; i < playerListSize; i++) {
		Player playerN;
		updateIn >> playerN;
		int id = playerN.getId();
        if(playerN.getIsAlive())
            movePlayer(id, playerN.getX(), playerN.getY());
        else if(playersItem[id])
        {
            //scene->removeItem(playersItem[playerN.getId()]->getItem());
            delete playersItem[id];
            playersItem[id] = NULL;
        }
	}

	qint8 newBombsListSize;
	updateIn >> newBombsListSize;
//	qDebug() << newBombsListSize << " new bombs received";
	for(qint8 i = 0; i < newBombsListSize; i++) {
		Bomb bombN;
		updateIn >> bombN;
		addBomb(bombN.playerId, bombN.x, bombN.y, bombN.bombId);
	}

	qint8 nbExplosions;
	updateIn >> nbExplosions;
	for(qint8 i = 0; i < nbExplosions; i++) {
		Flame* f = new Flame();
		updateIn >> *f;

		QList<qint16>::const_iterator itBomb = f->getFirstDetonatedBomb();
		for( ; itBomb != f->getLastDetonatedBomb(); ++itBomb)
			removeBomb(*itBomb);
        QSet<QPoint>::const_iterator itBrBlock = f->getFirstBrokenBlock();
        for( ; itBrBlock != f->getLastBrokenBlock(); ++itBrBlock)
        {
            //qDebug() << "GameArena brokenblock";
            int i = itBrBlock->x();
            int j = itBrBlock->y();
            map->setType(BlockMapProperty::broken, i, j);
            getCase(i,j)->setItem(pixmaps.getPixmap(map->getType(i,j)));
        }
		addFlame(f);
	}

    // list of players killed during this hearbeat
    QList<MapClient::killedPlayer> killedPlayers;
    updateIn >> killedPlayers;

    foreach(MapClient::killedPlayer frag, killedPlayers) {
        qint16 px, py;
        map->getPlayerPosition(frag.first, px, py);
        QGraphicsSquareItem* burnt = new QGraphicsSquareItem(px-squareSize/2, py-squareSize/2, squareSize);
        burnt->setItem(pixmaps.getPixmapBurnt());
        scene->addItem(burnt);
        burntPlayers.append(burnt);
        QTimer::singleShot(1500, this, SLOT(removeBurnt()));
    }
}

void GameArena::blockChanged(int i, int j)
{
	blockChanged(j * width + i);
}

const MapClient *GameArena::getMap()
{
	return map;
}

int GameArena::getWidth()
{
	return width;
}

int GameArena::getHeight()
{
	return height;
}

int GameArena::getNbPlayers() const
{
	return map->getMaxNbPlayers();
}

QGraphicsSquareItem *GameArena::getCase(int i, int j)
{
	return squaresItem[j*width+i];
}

QGraphicsSquareItem *GameArena::getCase(int pos)
{
	return squaresItem[pos];
}

QGraphicsSquareItem *GameArena::getPlayer(int id)
{
	return playersItem[id];
}

void GameArena::slotHearbeatUpdated(qint32 value) {
    int newTime = value / (1000 / MOVE_TICK_INTERVAL);

    // rounding for negative values offset by 1
    if(value < 0)
        --newTime;

    if(newTime != timeInSeconds) {
        timeInSeconds = newTime;
        emit sigTimeUpdated(newTime);
    }
}

void GameArena::removeBurnt() {
    QGraphicsSquareItem* item = burntPlayers.first();
    burntPlayers.pop_front();
    scene->removeItem(item);
    delete item;
}

GameArena::~GameArena()
{
	if(squaresItem)
	{
		for(int i = 0; i < width*height; i++)
			delete squaresItem[i];
		delete []squaresItem;
	}
	if(playersItem)
	{
		for(int i = 0; i < map->getMaxNbPlayers(); i++)
			delete playersItem[i];
		delete[] playersItem;
	}
    delete view;
    //delete scene; todo crash ?
}

