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
 * For now, the map create can only be a random map
 * Todo : add a load function, so we can load a map from a file
 */

template<typename P>
class Map
{
protected:
    Map();
    Map(qint16 w, qint16 h, qint16 block_size);

public:
    virtual ~Map();
    Map & operator=(const Map &);
    
    void Init();
    void setDim(qint16 w, qint16 h, qint16 block_size = BLOCK_SIZE);

    qint16 getWidth() const                                         { return width; }
    qint16 getHeight() const                                        { return height; }
    qint16 getBlockSize() const                                     { return blockSize; }
    BlockMapProperty::BlockType getType(int w, int h) const         { return block_list[h*width+w].getType(); }
    BlockMapProperty::BlockType getType(int pos) const              { return block_list[pos].getType(); }
    void setType(BlockMapProperty::BlockType type, int pos);
    void setType(BlockMapProperty::BlockType type, int x, int y);
    void getBlockPosition(int x, int y, int &xdest, int &ydest) const;

    void setPlayerPosition(int id, qint16 , qint16);
    void getPlayerPosition(int, qint16&, qint16&) const;
    void newPlayer(int id);
    qint8 getMaxNbPlayers() const                                   { return maxNbPlayers; }

    void addFlame(Flame* f);
    void removeFlame(int flameId);

    void addBomb(int id, int x, int y,int bombId);
    void removeBomb(qint16 bombId);
    bool blockContainsBomb(int x,int y) const;

	void setHeartBeat(qint32 hb);
	qint32 getHeartBeat() const                                     { return heartBeat; }

protected:
    BlockMapProperty* getBlockList() const                    { return block_list; }
    
private:
    qint16 width;
    qint16 height;
    qint16 blockSize;                       //There are "blockSize" pixels in one block
    BlockMapProperty *block_list;

    quint8 maxNbPlayers;

    //Test if a coordinate is below (-1) on (0) or above (1) the middle of the block
    int coordinatePositionInBlock(int coord);

protected:
	qint32 heartBeat;						///< timestamp of the game
	QList<P*> players;                      ///< list of players currently on the field
	QList<Bomb*> bombs;						///< list of bombs yet to explode
	QList<Flame*> flames;					///< list of explosions

    // signals
private:
    virtual void emitSigBlockChanged(int pos) = 0;
	virtual void emitSigBlockChanged(int i, int j) = 0;
    virtual void emitSigHeartbeatUpdated(qint32 value) = 0;
};


template<typename P>
Map<P>::Map() :
	width(0),
	height(0),
	blockSize(0),
    block_list(NULL),
    maxNbPlayers(0),
    heartBeat(-999999999)
{}

template<typename P>
Map<P>::Map(qint16 w, qint16 h, qint16 bs) :
	block_list(NULL),
	maxNbPlayers(0),
	heartBeat(-999999999)
{
	setDim(w,h,bs);
}

template<typename P>
Map<P>::~Map()
{
    delete[] block_list;

    foreach(P* p, players)
        delete p;

    foreach(Bomb* b, bombs)
        delete b;

    foreach(Flame* f, flames)
        delete f;
}

template<typename P>
Map<P>& Map<P>::operator=(const Map<P>& oldMap)
{
    if(this == &oldMap)
        return *this;
    setDim(oldMap.width, oldMap.height, oldMap.blockSize);
    for(int i = 0; i < width*height; i++)
        block_list[i].setType(oldMap.block_list[i].getType());

	//do not copy the pointers but the objects themselves
	foreach(const P* playerN, oldMap.players)
		players.append(new P(*playerN));

    return *this;
}


template<typename P>
void Map<P>::setDim(qint16 w, qint16 h, qint16 block_size)
{
    width = w;
    height = h;
    blockSize = block_size;
    Init();
}

template<typename P>
void Map<P>::Init()
{
    if( width * height <= 0 )
		return;

    delete[] block_list;
    block_list = new BlockMapProperty[width*height];

    qDebug() << "init";
}


template<typename P>
void Map<P>::setType(BlockMapProperty::BlockType type, int pos)
{
    block_list[pos].setType(type);
    emitSigBlockChanged(pos);
}

template<typename P>
void Map<P>::setType(BlockMapProperty::BlockType type, int x, int y)
{
    block_list[y*width+x].setType(type);
    emitSigBlockChanged(x,y);
}

template<typename P>
void Map<P>::getBlockPosition(int x, int y, int &xdest, int &ydest) const
{
    xdest = x / blockSize;
    ydest = y / blockSize;
}


template<typename P>
void Map<P>::getPlayerPosition(int pl, qint16 &x, qint16 &y) const
{
	x = players[pl]->getX();
	y = players[pl]->getY();
}

template<typename P>
void Map<P>::setPlayerPosition(int id, qint16 x, qint16 y)
{
	players[id]->setX(x);
	players[id]->setY(y);
}

template<typename P>
void Map<P>::newPlayer(int id)
{
    maxNbPlayers++;
    players.append(new P(id));
}


template<typename P>
void Map<P>::addFlame(Flame* flame)
{
	flames.append(flame);
    foreach (QPoint point, flame->getFlamePositions())
    {
        setType(BlockMapProperty::flame,point.x(),point.y());
    }
}

template<typename P>
void Map<P>::removeFlame(int flameId)
{
    QList<Flame*>::iterator itFlame = flames.begin();
    while(itFlame != flames.end()) {
        Flame *f = *itFlame;
        if(f->getFlameId() == flameId)
        {
            QSet<QPoint>::const_iterator it = f->getFirstFlame();

            //remove flames
            while(it != f->getLastFlame())
            {
                setType(BlockMapProperty::empty, (*it).x(), (*it).y());
                it++;
            }

            //remove broken blocks
            for( it = f->getFirstBrokenBlock(); it != f->getLastBrokenBlock(); it++)
            {
                //qDebug() << "GameArena brokenblock";
                setType(BlockMapProperty::empty, it->x(), it->y());
                //getCase(i,j)->setItem(pixmaps.getPixmap(map->getType(i,j)));
            }
            flames.erase(itFlame);
            delete f;
            break;
        }
        itFlame++;
    }
}


template<typename P>
void Map<P>::addBomb(int id, int squareX, int squareY, int bombId)
{
	Bomb *newBomb = new Bomb(id, squareX, squareY, bombId);
    bombs.append(newBomb);
    qDebug() << " Map> AddBomb : " << bombs.size() << " BOMBS !!! x: "<<squareX<<" y: "<<squareY<<"bombId: "<<newBomb->bombId;
    setType(BlockMapProperty::bomb,squareX,squareY);
}

template<typename P>
void Map<P>::removeBomb(qint16 bombId)
{
	foreach (Bomb *b, bombs)
	{
	  if(b->bombId == bombId)
	  {
		  bombs.removeOne(b);
		  setType(BlockMapProperty::bomb,b->x,b->y);
		  delete b;
		  return;
	  }
	}
	qDebug()<< "nothing has been removed";
}

template<typename P>
bool Map<P>::blockContainsBomb(int x,int y) const
{
   foreach( Bomb *b, bombs)
    {
    	if ((b->x == x) && (b->y == y))
    		return true;
    }
	return false;
}


template<typename P>
void Map<P>::setHeartBeat(qint32 hb) {
    heartBeat = hb;
    emitSigHeartbeatUpdated(hb);
}


template<typename P>
int Map<P>::coordinatePositionInBlock(int coord)
{
    int block = coord / blockSize;
    int middle = blockSize * block + blockSize/2;
    if(coord == middle)
        return 0;
    return coord - middle;
    //if(coord < middle)
    //    return -1;
    //return 1;    
}


template<typename P>
QDataStream &operator>>(QDataStream & in, Map<P> &map) {
    qint8 maxNbPlayers;
	qint16 width, height, blockSize;
    in >> maxNbPlayers >> width >> height >> blockSize;
    map.setDim(width, height, blockSize);

    //retreive player position from the data stream
    qint16 x,y;
    for(int i = 0; i < maxNbPlayers; i++)
    {
        in >> x >> y;
        map.newPlayer(i);
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

template<typename P>
QDataStream &operator<<(QDataStream &out, const Map<P> &map) {
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


#endif // QTB_MAP_H
