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

    connect(map,SIGNAL(sigMovePlayer(int, int, int)), this, SLOT(movePlayer(int, int, int)));
    connect(map,SIGNAL(sigKillPlayer(int)), this, SLOT(killPlayer(int)));
}

void GameArena::movePlayer(int player, int x, int y)
{
    Q_ASSERT(playersItem[player] != NULL); //Player really exists
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

void GameArena::killPlayer(int id)
{
	qint16 px, py;
	map->getPlayerPosition(id, px, py);
	scene->removeItem(playersItem[id]->getItem());
	delete playersItem[id];
	playersItem[id] = NULL;
	QGraphicsSquareItem* burnt = new QGraphicsSquareItem(px-squareSize/2, py-squareSize/2, squareSize);
	burnt->setItem(pixmaps.getPixmapBurnt());
	scene->addItem(burnt);
	burntPlayers.append(burnt);
	QTimer::singleShot(1500, this, SLOT(removeBurnt()));
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

