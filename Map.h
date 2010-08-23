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
 * For now, the map create can only be a random map
 * Todo : add a load function, so we can load a map from a file
 */

class Map : public QObject
{
    Q_OBJECT
private:
    qint16 width;
    qint16 height;
    BlockMapProperty *block_list;
    qint16 playersPositions[MAX_NB_PLAYER];
    void Init();

public:
    Map();
    Map(qint16, qint16);
    ~Map();
    void setDim(qint16,qint16);
    void loadRandom();
    qint16 getWidth() const ;
    qint16 getHeight() const;
    BlockMapProperty::BlockType getType(int,int) const;
    BlockMapProperty::BlockType getType(int) const;
    void setType(BlockMapProperty::BlockType type, int pos);
    void setPlayersPositions(const qint16 *);
    const BlockMapProperty* getBlockList() const;
    const qint16 *getPlayersPosition() const;
    void getPlayerPosition(int, int &, int &);
    void setPlayerPosition(int id, int pos);
    qint16 getMaxPlayer() const;
    bool movePlayer(int, int, int);
    Map & operator=(const Map &);

signals:
     void blockChanged(int pos);
};

QDataStream &operator>>(QDataStream & in, Map &map);
QDataStream &operator<<(QDataStream &out, const Map &map);

#endif

