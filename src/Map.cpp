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

#include <QtGlobal>
#include <QDataStream>
#include <QDebug>

#include "Map.h"
#include <stdlib.h> //Temporary (NULL)
#include <string.h>
#include <time.h> //qrand seed

Map::Map() :
	block_list(0),
	heartBeat(-999999999)
{
    setDim(0,0,0);
}

Map::Map(qint16 w, qint16 h, qint16 bs) :
	block_list(0),
	heartBeat(-999999999)
{
	setDim(w,h,bs);
}

void Map::Init()
{
    delete[] block_list;
    if( width * height > 0 )
    {
        block_list = new BlockMapProperty[width*height];
    }
    else
        block_list = NULL;

	for(int i = 0; i < MAX_NB_PLAYER; i++)
		players.append(new Player(i));

    qDebug() << "init";
}

void Map::setDim(qint16 w, qint16 h, qint16 bs)
{
    width = w;
    height = h;
    blockSize = bs;
    Init();
}

<<<<<<< HEAD
void Map::loadRandom()
{
    qsrand(time(NULL));
    for(int w = 1; w < width-1; w++)
    {
        for(int h = 1; h < height-1; h++)
        {
            //randomly add wall or bricks or nothing
            double d = (double)qrand() / RAND_MAX;
            if(d < 0.05)
                block_list[h*width+w].setType(BlockMapProperty::wall);
            else if(d < 0.1)
                block_list[h*width+w].setType(BlockMapProperty::brick);
            else
                block_list[h*width+w].setType(BlockMapProperty::empty);
        }
    }
    //add walls on the map sides
    for(int w = 0; w < width; w++)
    {
        block_list[w].setType(BlockMapProperty::wall);
        block_list[(height-1)*width+w].setType(BlockMapProperty::wall);
    }
    for(int h = 1; h < height-1; h++)
    {
        block_list[h*width].setType(BlockMapProperty::wall);
        block_list[h*width+width-1].setType(BlockMapProperty::wall);
    }
    //add players
	players[0]->setX(blockSize + blockSize/2); //middle of the second block
	players[0]->setY(blockSize + blockSize/2);

	players[1]->setX((width-2) * blockSize + blockSize/2);
	players[1]->setY((height-2) * blockSize + blockSize/2);

	players[2]->setX(blockSize + blockSize/2); //middle of the second block
	players[2]->setY((height-2) * blockSize + blockSize/2);

	players[3]->setX((width-2) * blockSize + blockSize/2);
	players[3]->setY(blockSize + blockSize/2);
 
	int x,y;
	foreach(const Player* playerN, players) {
		getBlockPosition(playerN->getX(), playerN->getY(), x, y);
		setType(BlockMapProperty::empty, x, y);
	}
}

=======
>>>>>>> 8f7b0bc28ad1b0e5e9e04e54534f1afab9968f95
BlockMapProperty::BlockType Map::getType(int w,int h) const
{
    return block_list[h*width+w].getType();
}

BlockMapProperty::BlockType Map::getType(int pos) const
{
    return block_list[pos].getType();
}

void Map::setType(BlockMapProperty::BlockType type, int pos)
{
    block_list[pos].setType(type);
}

void Map::setType(BlockMapProperty::BlockType type, int x, int y)
{
    block_list[y*width+x].setType(type);
}

void Map::setPlayerPosition(int id, qint16 x, qint16 y)
{
    int x_oldBlock, y_oldBlock;
	players[id]->setX(x);
	players[id]->setY(y);
    //emit playerMoved(id, x, y); useless?
}

void Map::flame(Flame& flame)
{
	flames.append(&flame);
}

void Map::removeFlame(int flameId)
{
	foreach (Flame *f, flames)
	{
	  if(f->getFlameId() == flameId)
	  {
		  flames.removeOne(f);
		  delete f;
		  return;
	  }
	}
}

Bomb* Map::bomb(int id, int squareX, int squareY, int bombId)
{
	Bomb *newBomb=NULL;
    newBomb = new Bomb(id, squareX, squareY, bombId) ;
    bombs.append(newBomb);
    qDebug() << " Map> AddBomb : " << bombs.size() << " BOMBS !!! x: "<<squareX<<" y: "<<squareY<<"bombId: "<<newBomb->bombId;
    return newBomb;
}

void Map::removeBomb(int bombId)
{
	foreach (Bomb *b, bombs)
	{
	  if(b->bombId == bombId)
	  {
		  bombs.removeOne(b);
		  delete b;
		  return;
	  }
	}
	qDebug()<< "nothing has been removed";
}




bool Map::blockContainsBomb(int x,int y)
{
   foreach( Bomb *b, bombs)
    {
    	if ((b->x == x) && (b->y == y))
    		return true;
    }
	return false;
}



int Map::coordinatePositionInBlock(int coord)
{
    int block = coord / blockSize;
    int middle = blockSize * block + blockSize/2;
    if(coord == middle)
        return 0;
    return coord - middle;
    //if(coord < middle)
    //    return -1;
    //return 1;    
}

qint16 Map::getWidth() const
{
    return width;
}

qint16 Map::getHeight() const
{
    return height;
}

void Map::getBlockPosition(int x, int y, int &xdest, int &ydest) const
{
    xdest = x / blockSize;
    ydest = y / blockSize;
}

void Map::getPlayerPosition(int pl, qint16 &x, qint16 &y) const
{
	x = players[pl]->getX();
	y = players[pl]->getY();
}
BlockMapProperty* Map::getBlockList()
{
    return block_list;
}
QList<Bomb*>* Map::getBombList(){
	return &bombs;
}

QList<Flame*>* Map::getFlameList(){
	return &flames;
}

qint16 Map::getBlockSize() const
{
    return blockSize;
}

qint16 Map::getMaxNbPlayers() const
{
    return MAX_NB_PLAYER;
}



Map::~Map()
{
    delete[] block_list;
}

QDataStream &operator<<(QDataStream &out, const Map &map)
{
    out << map.getMaxNbPlayers() << map.getWidth() << map.getHeight() << map.getBlockSize();
    //copy player positions in our data stream
    //out.writeBytes((const char *)map.getPlayersPosition(),map.getMaxNbPlayers()*sizeof(int));
    qint16 x,y;
    for(int i = 0; i < map.getMaxNbPlayers(); i++)
    {
        map.getPlayerPosition(i,x,y);
        out << x << y;
    }
    //copy block types in our data stream
    for(int i = 0; i < map.getWidth()*map.getHeight(); i++)
        out << (qint16)map.getType(i);
    return out;
}

QDataStream &operator>>(QDataStream & in, Map &map)
{
    qint16 maxNbPlayers, width, height, blockSize;
    in >> maxNbPlayers >> width >> height >> blockSize;
    map.setDim(width, height, blockSize);

    //retreive player position from the data stream
    qint16 x,y;
    for(int i = 0; i < maxNbPlayers; i++)
    {
        in >> x >> y;
        map.setPlayerPosition(i,x,y);
    }
    //retreive block types from the data stream
    for(int i = 0; i < width*height; i++)
    {
        in >> x;
        map.setType((BlockMapProperty::BlockType)x,i);
    }

    return in;
}

Map & Map::operator=(const Map &oldMap)
{
    if(this == &oldMap)
        return *this;
    setDim(oldMap.width, oldMap.height, oldMap.blockSize);
    for(int i = 0; i < width*height; i++)
        block_list[i].setType(oldMap.block_list[i].getType());

	//do not copy the pointers but the objects themselves
	foreach(const Player* playerN, oldMap.players)
		players.append(new Player(*playerN));

    return *this;
}

