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

#ifndef MAPSERVER_H
#define MAPSERVER_H


#include "Map.h"
#include "Flame.h"
#include "Bomb.h"

/**
 * This class represent a bomberman game map
 * For now, the map create can only be a random map
 * Todo : add a load function, so we can load a map from a file
 */

class MapServer : public Map
{
	 Q_OBJECT
private:



    void adjustPlayerPosition(int plId, int xDirection, int yDirection);
    //Test if a coordinate is bellow (-1) on (0) or above (1) the middle of the block
    int coordinatePositionInBlock(int coord);
    int absMin(int a, int b) const;
    bool movePlayerLeft(int id);
    bool movePlayerRight(int id);
    bool movePlayerUp(int id);
    bool movePlayerDown(int id);
    bool movePlayerOld(int id, int direction);
public:
//    MapServer();
//    MapServer(qint16, qint16, qint16);
//    ~MapServer();
    void loadRandom();
    bool movePlayer(int id, int direction);
    int bomb(int id, int x, int y);
    int bomb(int id);
private slots :
void explosion(Bomb* b);

signals:
	void addFlame(Flame& f);
    void bombRemoved(int bombId);
};



#endif

