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


#include <QObject>
#include <QList>
#include <QDebug>

#include "BlockMapProperty.h"
#include "constant.h"
#include "Bomb.h"
#include "Flame.h"


/**
 * This class represent a bomberman game map
 */

// WARNING! This class inherits from QObject but does not use the Q_OBJECT macro
// WARNING! This is needed to allow the use of a template class, and should not
// WARNING! pose a problem since it is an abstract one (the methods to be used
// WARNING! as signals are pure virtual ones).

//P : players
//SL : style list
template<typename P, typename SL>
class Map : public QObject
{
protected:
    Map();
    Map(qint16 w, qint16 h, qint16 block_size);

public:

    virtual ~Map();
    
    void Init();
    void setDim(qint16 w, qint16 h, qint16 block_size = BLOCK_SIZE);
    virtual void addStyle(const SL &style)=0;

    qint16 getWidth() const                                         { return width; }
    qint16 getHeight() const                                        { return height; }
    qint16 getBlockSize() const                                     { return blockSize; }
    BlockMapProperty::BlockType getType(int w, int h) const         { return block_list[h*width+w].getType(); }
    BlockMapProperty::BlockType getType(int pos) const              { return block_list[pos].getType(); }
    void setType(BlockMapProperty::BlockType type, int pos);
    void setType(BlockMapProperty::BlockType type, int x, int y);
    void setOption(int pos, BlockMapProperty::BlockOption option, BlockMapProperty::optionDirection dir);
    void setOption(int x, int y, BlockMapProperty::BlockOption option, BlockMapProperty::optionDirection dir);
    BlockMapProperty::BlockOption getOption(int w, int h) const     { return block_list[h*width+w].getOption(); }
    BlockMapProperty::BlockOption getOption(int pos) const          { return block_list[pos].getOption(); }
    BlockMapProperty::optionDirection getOptionDirection(int w, int h) const     { return block_list[h*width+w].getOptionDirection(); }
    BlockMapProperty::optionDirection getOptionDirection(int pos) const          { return block_list[pos].getOptionDirection(); }

    void getBlockPosition(int x, int y, int &xdest, int &ydest) const;
    QPoint getCenterCoordForBlock(int x, int y) const;
    void getNextBlock(int x, int y, int &xdest, int &ydest, int direction) const;
    const QList<SL> *getStylesList() const;

    void setPlayerPosition(int id, qint16 , qint16);
    bool setPlayerSickness(int id, bool sick);                     //return true if sickness changed
    void getPlayerPosition(int, qint16&, qint16&) const;
    void newPlayer(int id);
    qint8 getMaxNbPlayers() const                                   { return maxNbPlayers; }
    qint8 getNbPlayers() const                                      { return NbPlayers; }
    void setMaxNbPlayers(int);

    void addFlame(Flame* f);
    void removeFlame(int flameId);

    void addBomb(int plId, int x, int y,int bombId, bool rc);
    void removeBomb(qint16 bombId);
    const Bomb *getBomb(qint16 bombId);
    Bomb *blockContainsBomb(int x,int y) const;

	void setHeartBeat(qint32 hb);
	qint32 getHeartBeat() const                                     { return heartBeat; }

    typedef QPair<qint8,qint8> killedPlayer;       // T1: who was killed, T2: by who
    
protected:
    quint8 NbPlayers;
    quint8 maxNbPlayers;
    qint16 width;
    qint16 height;
    QList<SL> styles; //list of map styles available
    BlockMapProperty *block_list;

    static const int MOVE_LEFT = 0;
    static const int MOVE_UP = 1;
    static const int MOVE_RIGHT = 2;
    static const int MOVE_DOWN = 3;

private:
    // callback method for when a brick wall is removed, only useful for the server
    virtual void brokenBlockRemoved(int, int)                       {}


    qint16 blockSize;                       //There are "blockSize" pixels in one block

    //Test if a coordinate is below (<0) on (0) or above (>0) the middle of the block
    int coordinatePositionInBlock(int coord);

protected:
	qint32 heartBeat;						///< timestamp of the game
	QList<P*> players;                      ///< list of players currently on the field
	QList<Bomb*> bombs;						///< list of bombs yet to explode
	QList<Flame*> flames;					///< list of explosions

    // signals
private:
    virtual void sigBlockChanged(int pos) = 0;
	virtual void sigBlockChanged(int i, int j) = 0;
    virtual void sigHeartbeatUpdated(qint32 value) = 0;
};

#include "Map.cpp"

#endif // QTB_MAP_H
