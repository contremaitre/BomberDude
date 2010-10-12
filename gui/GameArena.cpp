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
#include <QGraphicsView>
#include <QMainWindow>
#include <QDataStream>
#include <QList>
#include "Flame.h"
#include "GameArena.h"

GameArena::GameArena(QMainWindow * mainw, int s)
{
	width = height = 0;
	squareSize = s;
	squaresItem = NULL;
	playersItem = NULL;
    scene = new QGraphicsScene;
    mainWindow = mainw;
    view = NULL;
	connect(&map,SIGNAL(blockChanged(int)),this,SLOT(blockChanged(int)));
	connect(&map,SIGNAL(blockChanged(int,int)),this,SLOT(blockChanged(int,int)));
	loadPixMaps();
}

void GameArena::loadPixMaps()
{
	pixmaps.init(squareSize, squareSize);
}

void GameArena::init()
{
	width = map.getWidth();
	height = map.getHeight();
	delete[] squaresItem;
	delete[] playersItem;
	squaresItem = new QGraphicsSquareItem*[width * height];
	playersItem = new QGraphicsSquareItem*[map.getMaxNbPlayers()];
	for(int i = 0; i < width; i++)
	{
		for(int j = 0; j < height; j++)
		{
			initCase(i,j);
			getCase(i,j)->setItem(pixmaps.getPixmap(map.getType(i,j)));
		}
	}
	qint16 x,y;
	for(int i = 0; i < map.getMaxNbPlayers(); i++)
	{
		map.getPlayerPosition(i,x,y);
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
}

void GameArena::getEventFilter(QObject *obj)
{
    scene->installEventFilter(obj);
}

void GameArena::setMap(const Map *map)
{
	this->map = *map;
	init();
}

void GameArena::movePlayer(int player, int x, int y)
{
	map.setPlayerPosition(player, x, y);
	playersItem[player]->setPos(x-squareSize/2,y-squareSize/2,squareSize);
}

void GameArena::addBomb(int player, int squareX, int squareY, int bombId)
{
	int x,y;
	x=squareX*squareSize;
	y=squareY*squareSize;
	Bomb* bomb=map.bomb(player, squareX, squareY, bombId);
	qDebug()<<"add a bomb"<<bomb;

	if (bomb)
	{
		QGraphicsSquareItem* bombItem=new QGraphicsSquareItem(x,y,squareSize);
		bombItem->setItem(pixmaps.getPixmap(BlockMapProperty::bomb));

		bombsItem.insert(bombId,bombItem);
		scene->addItem(bombItem);

	}
}

void GameArena::addFlame(Flame& flame)
{
	map.flame(flame);
	QList<QGraphicsSquareItem*> *flameItems=new QList<QGraphicsSquareItem*>();

	foreach (QPoint point, flame.getFlamePositions())
	{
		QGraphicsSquareItem* item = new QGraphicsSquareItem(point.x() * squareSize, point.y() * squareSize, squareSize);
		item->setItem(pixmaps.getPixmap(BlockMapProperty::flame));
		flameItems->append(item);
		scene->addItem(item);
	}
	flamesItem.insert(flame.getFlameId(),flameItems);
}


void GameArena::removeBomb(qint16 bombId)
{
	map.removeBomb(bombId);
	QGraphicsSquareItem * itemToRemove=bombsItem.value(bombId);
    scene->removeItem(itemToRemove);
	bombsItem.remove(bombId);
	delete itemToRemove;
}

void GameArena::removeFlame(int flameId)
{
	map.removeFlame(flameId);
	QList<QGraphicsSquareItem *>* itemsToRemove=flamesItem.value(flameId);
	qDebug()<< "GameArena> removeFlame";
    foreach(QGraphicsSquareItem * item, *itemsToRemove)
	{
        //qDebug()<< "GameField> flameRemoved(2)";
        scene->removeItem(item);
	}
	flamesItem.remove(flameId);
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
	qDebug() << "Wall: " << tempItem->getItem()->pixmap().cacheKey() << ", new value: " << pixmaps.getPixmap(map.getType(pos)).cacheKey();
	tempItem->setItem(pixmaps.getPixmap(map.getType(pos)));
	scene->removeItem(tempItem);
	scene->addItem(tempItem);
	emit pixmapChanged(pos);
}

void GameArena::updateMap(QByteArray& updateBlock) {
	QDataStream updateIn(updateBlock);

	qint32 heartBeat;
	updateIn >> heartBeat;
	map.setHeartBeat(heartBeat);
	qDebug() << "Received heartbeat " << heartBeat;

	QList<qint16> cleanList;
	updateIn >> cleanList;
	foreach(qint16 flameId, cleanList)
		removeFlame(flameId);

	qint8 playerListSize;
	updateIn >> playerListSize;
	qDebug() << playerListSize << " players received";
	for(qint8 i = 0; i < playerListSize; i++) {
		Player playerN;
		updateIn >> playerN;
		qDebug() << "Player #" << playerN.getId() << ", x:" << playerN.getX() << " y:" << playerN.getY();
		movePlayer(playerN.getId(), playerN.getX(), playerN.getY());
	}

	qint8 newBombsListSize;
	updateIn >> newBombsListSize;
	qDebug() << newBombsListSize << " new bombs received";
	for(qint8 i = 0; i < newBombsListSize; i++) {
		Bomb bombN;
		updateIn >> bombN;
		qDebug() << "Bomb #" << bombN.bombId << ", player #" << bombN.playerId << ", x:" << bombN.x << ", y:" << bombN.y;
		addBomb(bombN.playerId, bombN.x, bombN.y, bombN.bombId);
	}

	qint8 nbExplosions;
	updateIn >> nbExplosions;
	for(qint8 i = 0; i < nbExplosions; i++) {
		Flame f;
		updateIn >> f;

		QList<qint16>::const_iterator itBomb = f.getFirstDetonatedBomb();
		for( ; itBomb != f.getLastDetonatedBomb(); ++itBomb)
			removeBomb(*itBomb);

		addFlame(f);
	}
}

void GameArena::blockChanged(int i, int j)
{
	blockChanged(j * width + i);
}

const Map *GameArena::getMap()
{
	return &map;
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
	return map.getMaxNbPlayers();
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
		for(int i = 0; i < map.getMaxNbPlayers(); i++)
			delete playersItem[i];
		delete[] playersItem;
	}
    delete view;
    //delete scene; todo crash ?
}

