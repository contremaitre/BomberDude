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
#include <stdlib.h> //temporaraire (NULL)
#include <string.h>
#include <time.h> //qrand seed


Map::Map()
{
    block_list = NULL;
    setDim(0,0,0);
}

Map::Map(qint16 w, qint16 h, qint16 bs)
{
    block_list = NULL;
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
    memset(playersPositions,0,sizeof(playersPositions));
}

void Map::setDim(qint16 w, qint16 h, qint16 bs)
{
    width = w;
    height = h;
    blockSize = bs;
    Init();
}

void Map::loadRandom()
{
    qsrand(time(NULL));
    for(int w = 1; w < width-1; w++)
    {
        for(int h = 1; h < height-1; h++)
        {
            //randomly add wall or bricks or nothing
            double d = (double)qrand() / RAND_MAX;
            if(d < 0.1)
                block_list[h*width+w].setType(BlockMapProperty::wall);
            else if(d < 0.4)
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
    playersPositions[0].x = blockSize + blockSize/2; //middle of the second block
    playersPositions[0].y = blockSize + blockSize/2;
    playersPositions[1].x = (width-2) * blockSize + blockSize/2;
    playersPositions[1].y = (height-2) * blockSize + blockSize/2;
    int x,y;
    getBlockPosition(playersPositions[0].x,playersPositions[0].y,x,y);
    setType(BlockMapProperty::empty,x,y);
    getBlockPosition(playersPositions[1].x,playersPositions[1].y,x,y);
    setType(BlockMapProperty::empty,x,y);
}

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
    playersPositions[id].x = x;
    playersPositions[id].y = y;
    emit playerMoved(id, x, y);
}

/**
 *      1
 *      |
 *  0 <- -> 2
 *      |
 *      3
 */
bool Map::movePlayer(int id, int direction)
{
    /**
     * Rules for a player move :
     * Get the block where the player is and :
     * - 1 if the player is at or beyond the middle of the block (wrt the direction of the move),
     * and if the next block is not empty, we refuse to move
     * - 2 else the move is accepted
     * - TODO : Gerer le contournement des blocks comme dans AB
     */
    qint16 x,y;
    int move_x = 0;
    int move_y = 0;
    getPlayerPosition( id, x, y );
    //qDebug() << "Map move player" << id << direction << "x,y" << x << y;
    switch(direction)
    {
        case 0:
            move_x = -1;
        break;
        case 1:
            move_y = -1;
        break;
        case 2:
            move_x = 1;
        break;
        case 3:
            move_y = 1;
        break;
    }
    x += move_x + move_x*(blockSize/2);
    y += move_y + move_y*(blockSize/2);
    int x_nextBlock, y_nextBlock;
    getBlockPosition( x, y, x_nextBlock, y_nextBlock );
    //qDebug() << "next block" << x_nextBlock << y_nextBlock ;
    BlockMapProperty::BlockType type = getType(x_nextBlock,y_nextBlock);
    if( type == BlockMapProperty::empty )
    {
        playersPositions[id].x += move_x;
        playersPositions[id].y += move_y;
        return true;
    }
    return false;
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
    x = playersPositions[pl].x;
    y = playersPositions[pl].y;
}

const BlockMapProperty* Map::getBlockList() const
{
    return block_list;
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
    for(int i = 0; i < getMaxNbPlayers(); i++)
        playersPositions[i] = oldMap.playersPositions[i];
//    memcpy(playersPositions, oldMap.playersPositions, sizeof(*playersPositions)*getMaxNbPlayers()));
    return *this;
}

