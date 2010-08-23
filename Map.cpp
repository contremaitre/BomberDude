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

#include "Map.h"
#include <stdlib.h> //temporaraire (NULL)
#include <string.h>
#include <time.h> //qrand seed


Map::Map()
{
    block_list = NULL;
    setDim(0,0);
}

Map::Map(qint16 w, qint16 h)
{
    block_list = NULL;
    setDim(w,h);
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

void Map::setDim(qint16 w, qint16 h)
{
    width = w;
    height = h;
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
    playersPositions[0] = width+1;
    playersPositions[1] = (height-2)*width + width-2;
    block_list[playersPositions[0]].setType(BlockMapProperty::player);
    block_list[playersPositions[1]].setType(BlockMapProperty::player);
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

void Map::setPlayerPosition(int id, int pos)
{
    int old_pos = playersPositions[id];
    playersPositions[id] = pos;
    //check if another player was here
    int other_id = -1;
    for(int i = 0; i < MAX_NB_PLAYER; i++)
    {
        if(playersPositions[i] == old_pos)
        {
            other_id = i;
            break;
        }
    }
    if(other_id != -1)
    {
        //todo : we need to update the pixmap to match this player
    }
    else
    {
        block_list[old_pos].setType(BlockMapProperty::empty);
        emit blockChanged(old_pos);
        block_list[pos].setType(BlockMapProperty::player);
        emit blockChanged(pos);
    }
}

bool Map::movePlayer(int id, int x, int y)
{
    if(x < 0 || x >= width || y < 0 || y >= height)
        return false;
    BlockMapProperty::BlockType type = getType(x,y);
    if( type == BlockMapProperty::player || type == BlockMapProperty::empty )
    {
        setPlayerPosition(id,y*width+x);
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

qint16 Map::getPlayerPosition(int pos) const
{
    return playersPositions[pos];
}

void Map::getPlayerPosition(int pl, int &x, int &y) const
{
    x = playersPositions[pl] % width;
    y = playersPositions[pl] / width;
}

const BlockMapProperty* Map::getBlockList() const
{
    return block_list;
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
    out << map.getMaxNbPlayers() << map.getWidth() << map.getHeight();
    //copy player positions in our data stream
    //out.writeBytes((const char *)map.getPlayersPosition(),map.getMaxNbPlayers()*sizeof(int));
    for(int i = 0; i < map.getMaxNbPlayers(); i++)
        out << map.getPlayerPosition(i);
    //copy block types in our data stream
    for(int i = 0; i < map.getWidth()*map.getHeight(); i++)
        out << (qint16)map.getType(i);
    return out;
}

QDataStream &operator>>(QDataStream & in, Map &map)
{
    qint16 maxNbPlayers, width, height;
    in >> maxNbPlayers >> width >> height;
    map.setDim(width, height);

    //retreive player position from the data stream
    qint16 var;
    for(int i = 0; i < maxNbPlayers; i++)
    {
        in >> var;
        map.setPlayerPosition(i,var);
    }
    //retreive block types from the data stream
    for(int i = 0; i < width*height; i++)
    {
        in >> var;
        map.setType((BlockMapProperty::BlockType)var,i);
    }

    return in;
}

Map & Map::operator=(const Map &oldMap)
{
    if(this == &oldMap)
        return *this;
    setDim(oldMap.width, oldMap.height);
    for(int i = 0; i < width*height; i++)
        block_list[i].setType(oldMap.block_list[i].getType());
    for(int i = 0; i < getMaxNbPlayers(); i++)
        playersPositions[i] = oldMap.playersPositions[i];
//    memcpy(playersPositions, oldMap.playersPositions, sizeof(*playersPositions)*getMaxNbPlayers()));
    return *this;
}

