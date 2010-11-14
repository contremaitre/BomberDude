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

#ifndef QTB_MAP_H
#define QTB_MAP_H


#include <QList> 
#include <QObject>

#include "BlockMapProperty.h"
#include "constant.h"
#include "Bomb.h"
#include "Flame.h"
#include "Player.h"

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
    //There are "blockSize" pixels in one block
    qint16 blockSize;
    BlockMapProperty *block_list;
    quint8 maxNbPlayers;
    void Init();
    //Test if a coordinate is bellow (-1) on (0) or above (1) the middle of the block
    int coordinatePositionInBlock(int coord);

protected:
	qint32 heartBeat;						///< timestamp of the game
	QList<Player*> players;					///< list of players currently on the field
	QList<Bomb*> bombs;						///< list of bombs yet to explode
	QList<Flame*> flames;					///< list of explosions

public:
    Map();
    Map(qint16, qint16, qint16);
    ~Map();
    void setDim(qint16,qint16,qint16 block_size = BLOCK_SIZE);
    qint16 getWidth() const ;
    qint16 getHeight() const;
    BlockMapProperty::BlockType getType(int,int) const;
    BlockMapProperty::BlockType getType(int) const;
    void setType(BlockMapProperty::BlockType type, int pos);
    void setType(BlockMapProperty::BlockType type, int x, int y);
    BlockMapProperty* getBlockList() ;
    //get the block number at coordinate x,y
    void getBlockPosition(int, int, int&, int&) const;
    //get the coordinates of the player
    void getPlayerPosition(int, qint16&, qint16&) const;
    void setPlayerPosition(int id, qint16 , qint16);
    void newPlayer(int id);                            //add a player to the game
    qint8 getMaxNbPlayers() const;
    qint16 getBlockSize() const;
    void addFlame(Flame* f);
    void removeFlame(int flameId);
    void addBomb(int id, int x, int y,int bombId);
    void removeBomb(qint16 bombId);
    bool blockContainsBomb(int x,int y);
    Map & operator=(const Map &);

	void setHeartBeat(qint32 hb)							{ heartBeat = hb; }
	qint32 getHeartBeat() const								{ return heartBeat; }

signals:
    void blockChanged(int pos);
	void blockChanged(int i, int j);
};

QDataStream &operator>>(QDataStream & in, Map &map);
QDataStream &operator<<(QDataStream &out, const Map &map);


#endif // QTB_MAP_H
