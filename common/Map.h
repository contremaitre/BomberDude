/*
    Copyright (C) 2010,2011 Sébastien Escudier

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


#include <QVector>
#include <QObject>
#include <QList>
#include <QDebug>

#include "BlockMapProperty.h"
#include "constant.h"
#include "Flame.h"
#include "Tile.h"


/**
  * This class is used as template parameter for other classes
  * that aren't supposed to know about the internals of pixels
  * and tiles coordinates.
  */
class PixelToBlock
{
public:
    PixelToBlock(qint16 blockSize) :
        blockSize(blockSize)
    {}

    qint8 operator()(qint16 val) {
        return val / blockSize;
    }

private:
    const qint16 blockSize;
};


/**
 * This class represent a bomberman game map
 */

// WARNING! This class inherits from QObject but does not use the Q_OBJECT macro
// WARNING! This is needed to allow the use of a template class, and should not
// WARNING! pose a problem since it is an abstract one (the methods to be used
// WARNING! as signals are pure virtual ones).

//P : players
//B : bombs
//SL : style list
template<typename P, typename B, typename SL>
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
    void setOption(int pos, BlockMapProperty::BlockOption option, globalDirection dir);
    void setOption(int x, int y, BlockMapProperty::BlockOption option, globalDirection dir);
    BlockMapProperty::BlockOption getOption(int w, int h) const     { return block_list[h*width+w].getOption(); }
    BlockMapProperty::BlockOption getOption(int pos) const          { return block_list[pos].getOption(); }
    globalDirection getOptionDirection(int w, int h) const     { return block_list[h*width+w].getOptionDirection(); }
    globalDirection getOptionDirection(int pos) const          { return block_list[pos].getOptionDirection(); }

    QPoint getBlockPosition(int x, int y) const;

    /**
      Gives the "pixel" coordinates of the center of the given tile.

      @param x abscissa in tiles
      @param y ordinate in tiles
      @returns a QPoint with the block center's coordinates in pixels
      */
    QPoint getCenterCoordForBlock(int x, int y) const;

    /**
      Finds the nearest tile from the given coordinates, i.e. finds on which tile
      the sprite overlaps even though it is still technically in the tile "before".

      Necessary for removing bonuses when bombs move, because as soon as the sprite
      touches the bonus, the latter disappears (cf AB). If the object's coordinates
      are in the middle of the tile, i.e. the sprite is centered on its tile, then
      it's the tile itself which is returned, not a neighbour.

      @param x abscissa in pixels
      @param y ordinate in pixels
      @returns a QPoint with the block's coordinates
     */
    QPoint getOverlappingBlockPosition(int x, int y) const;

    void getNextBlock(int x, int y, int &xdest, int &ydest, globalDirection direction, bool loop = false) const;
    const QList<SL> *getStylesList() const;

    void setPlayerPosition(int id, qint16 , qint16);
    bool setPlayerSickness(int id, bool sick);                     //return true if sickness changed
    void getPlayerPosition(int, qint16&, qint16&) const;
    void newPlayer(int id);
    qint8 getMaxNbPlayers() const                                   { return maxNbPlayers; }
    qint8 getNbPlayers() const                                      { return NbPlayers; }
    void setMaxNbPlayers(int);

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

private:
    // callback method for when a brick wall is removed, only useful for the server
    virtual void brokenBlockRemoved(int, int)                       {}


    qint16 blockSize;                       //There are "blockSize" pixels in one block

    //Test if a coordinate is below (<0) on (0) or above (>0) the middle of the block
    int coordinatePositionInBlock(int coord);

protected:
	qint32 heartBeat;						///< timestamp of the game
	QMap<typename P::playerId_t, P*> players;   ///< list of players currently on the field

// contents of tiles
private:
    QVector<QVector<Tile<P,B> > > tiles;         ///< contents of each tile
protected:
    B* getTileBomb(qint8 tile_x, qint8 tile_y) const;
    const QSet<Flame*> getTileFlames(qint8 tile_x, qint8 tile_y);
    Bonus* getTileBonus(qint8 tile_x, qint8 tile_y) const;
    void setTileBonus(qint8 tile_x, qint8 tile_y, Bonus* b);

// list of flames
private:
    QMap<Flame::flameId_t, Flame*> flames;      ///< list of explosions
protected:
    void addFlame(Flame* f);
    void removeFlame(qint16 flameId);
    const QMap<Flame::flameId_t, Flame*>& getFlameList() const      { return flames; }

// list of bombs
private:
	QMap<typename B::bombId_t, B*> bombs;          ///< list of bombs yet to explode
protected:
    void addBomb(B* b);
    void removeBomb(qint16 bombId);
    B* getBomb(qint16 bombId) const;
    const QMap<typename B::bombId_t, B*>& getBombList() const       { return bombs; }

    // signals
private:
    virtual void sigBlockChanged(int pos) = 0;
	virtual void sigBlockChanged(int i, int j) = 0;
    virtual void sigHeartbeatUpdated(qint32 value) = 0;

    // slots
protected:
    virtual void slotBombTileChanged(qint16 bombId, qint8 oldx, qint8 oldy, qint8 newx, qint8 newy);
    virtual void slotFlyingBombChange(qint16 bombId);

};

// FIXME it's not exactly a cpp file (doesn't give a .o), better extension name for it?

#include "Map.cpp"

#endif // QTB_MAP_H
