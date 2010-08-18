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

#ifndef MAP_H
#define MAP_H

#include "BlockMapProperty.h"
#include <QObject>
#include "constant.h"

/**
 * This class represent a bomberman game map
 * For now, the map is hardcoded.
 * Todo : add a load function, so we can load a map from a file
 */

class Map : public QObject
{
    Q_OBJECT
private:
    int width;
    int height;
    BlockMapProperty *block_list;
    int players_positions[MAX_NB_PLAYER];
    void Init();
public:
    Map();
    Map(int, int);
    ~Map();
    void setDim(int,int);
    void loadRandom();
    int getWidth();
    int getHeight();
    BlockMapProperty::BlockType getType(int,int);
    BlockMapProperty::BlockType getType(int);
    int *getPlayersPosition();
    void setPlayerPosition(int, int);

signals:
     void blockChanged(int pos);
};

#endif

