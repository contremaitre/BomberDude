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
#include "Area.h"

Area::Area(int s)
{
	width = height = 0;
	squareSize = s;
	squaresItem = NULL;
	playersItem = NULL;
	connect(&map,SIGNAL(blockChanged(int)),this,SLOT(blockChanged(int)));
	loadPixMaps();
}

void Area::loadPixMaps()
{
	pixmaps.init(squareSize, squareSize);
}

void Area::init()
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

void Area::setMap(const Map *map)
{
	this->map = *map;
	init();
}

void Area::createRandomMap(int w, int h)
{
	map.setDim(w,h,squareSize);
	map.loadRandom();
	init();
}


void Area::movePlayer(int player, int x, int y)
{
	map.setPlayerPosition(player, x, y);
	playersItem[player]->setPos(x-squareSize/2,y-squareSize/2,squareSize);
}
void Area::addBomb(int player, int squareX, int squareY)
{
	int x,y;
	x=squareX*squareSize;
	y=squareY*squareSize;
	Bomb* bomb=map.bomb(player, squareX, squareY);
	qDebug()<<"add a bomb"<<bomb;

	if (bomb)
	{
		QGraphicsSquareItem* bombItem=new QGraphicsSquareItem(x,y,squareSize);
		bombItem->setItem(pixmaps.getPixmap(BlockMapProperty::bomb));

		bombsItem.insert(bomb,bombItem);
		emit bombAdded(bombItem);

	}
}
void Area::removeBomb(int x, int y)
{
	Bomb* bomb=map.removeBomb(x,y);
	QGraphicsSquareItem * itemToRemove=bombsItem.value(bomb);
	emit bombRemoved(itemToRemove);
	bombsItem.remove(bomb);


}
int Area::getCaseSize() const
{
	return squareSize;
}

void Area::initCase(int i, int j)
{
	int x_a = i*squareSize;
	int y_a = j*squareSize;
	squaresItem[j*width+i] = new QGraphicsSquareItem(x_a,y_a,squareSize);
}

void Area::blockChanged(int pos)
{
	getCase(pos)->setItem(pixmaps.getPixmap(map.getType(pos)));
	emit pixmapChanged(pos);
}

const Map *Area::getMap()
{
	return &map;
}

int Area::getWidth()
{
	return width;
}

int Area::getHeight()
{
	return height;
}

int Area::getNbPlayers() const
{
	return map.getMaxNbPlayers();
}

QGraphicsSquareItem *Area::getCase(int i, int j)
{
	return squaresItem[j*width+i];
}

QGraphicsSquareItem *Area::getCase(int pos)
{
	return squaresItem[pos];
}

QGraphicsSquareItem *Area::getPlayer(int id)
{
	return playersItem[id];
}

Area::~Area()
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
}

