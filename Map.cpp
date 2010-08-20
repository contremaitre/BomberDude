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

#include "Map.h"
#include <stdlib.h> //temporaraire (NULL)
#include <string.h>
#include <time.h> //qrand seed
#include <QtGlobal>

Map::Map()
{
    block_list = NULL;
    setDim(0,0);
}

Map::Map(int w, int h)
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
    memset(players_positions,0,sizeof(players_positions));
}

void Map::setDim(int w, int h)
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
    players_positions[0] = width+1;
    players_positions[1] = (height-2)*width + width-2;
    block_list[players_positions[0]].setType(BlockMapProperty::player);
    block_list[players_positions[1]].setType(BlockMapProperty::player);
}

BlockMapProperty::BlockType Map::getType(int w,int h) const
{
    return block_list[h*width+w].getType();
}

BlockMapProperty::BlockType Map::getType(int pos) const
{
    return block_list[pos].getType();
}

void Map::setPlayerPosition(int id, int pos)
{
    int old_pos = players_positions[id];
    players_positions[id] = pos;
    //check if another player was here
    int other_id = -1;
    for(int i = 0; i < MAX_NB_PLAYER; i++)
    {
        if(players_positions[i] == old_pos)
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



int Map::getWidth()
{
    return width;
}

int Map::getHeight()
{
    return height;
}

void Map::getPlayerPosition(int pl, int &x, int &y)
{
    x = players_positions[pl] % width;
    y = players_positions[pl] / width;
}

int *Map::getPlayersPosition()
{
    return players_positions;
}

int Map::getMaxPlayer() const
{
    return MAX_NB_PLAYER;
}

Map::~Map()
{
    delete[] block_list;
}

Map & Map::operator=(const Map &oldMap)
{
    if(this == &oldMap)
        return *this;
    setDim(oldMap.width, oldMap.height);
    memcpy(block_list,oldMap.block_list,sizeof(*block_list)*width*height);
    memcpy(players_positions, oldMap.players_positions, sizeof(players_positions));
    return *this;
}

