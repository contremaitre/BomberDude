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

#include <QList> 
#include <QObject>
#include "BlockMapProperty.h"
#include "constant.h"
#include "Bomb.h"

/**
 * This class represent a bomberman game map
 * For now, the map create can only be a random map
 * Todo : add a load function, so we can load a map from a file
 */

class Map : public QObject
{
    Q_OBJECT
private:
    qint16 width;
    qint16 height;
    qint16 blockSize;
    BlockMapProperty *block_list;
    //player position coordinates is in "pixel". And there is "blockSize" pixels in one block
    struct point{qint16 x; qint16 y;};
    point playersPositions[MAX_NB_PLAYER];
    QList<Bomb*> bombs;

    void Init();
    //Test if a coordinate is bellow (-1) on (0) or above (1) the middle of the block
    int coordinatePositionInBlock(int coord);

public:
    Map();
    Map(qint16, qint16, qint16);
    ~Map();
    void setDim(qint16,qint16,qint16);
    void loadRandom();
    qint16 getWidth() const ;
    qint16 getHeight() const;
    BlockMapProperty::BlockType getType(int,int) const;
    BlockMapProperty::BlockType getType(int) const;
    void setType(BlockMapProperty::BlockType type, int pos);
    void setType(BlockMapProperty::BlockType type, int x, int y);
    BlockMapProperty* getBlockList() ;
    QList<Bomb*>* getBombList();
    //get the block number at coordinate x,y
    void getBlockPosition(int, int, int&, int&) const;
    //get the coordinates of the player
    void getPlayerPosition(int, qint16&, qint16&) const;
    void setPlayerPosition(int id, qint16 , qint16);
    qint16 getMaxNbPlayers() const;
    qint16 getBlockSize() const;

    Bomb* bomb(int id, int x, int y,int bombId);
    Bomb* removeBomb(int bombId);
    bool blockContainsBomb(int x,int y);
    Map & operator=(const Map &);

signals:
    void blockChanged(int pos);
    void bombRemoved(int bombId);
    //void playerMoved(int pl, int x, int y); useless?

};

QDataStream &operator>>(QDataStream & in, Map &map);
QDataStream &operator<<(QDataStream &out, const Map &map);

#endif

