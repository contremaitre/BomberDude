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

#include <Map.h>

template<typename P>
Map<P>::Map() :
    NbPlayers(0),
    maxNbPlayers(0),
    width(0),
    height(0),
    blockSize(0),
    block_list(NULL),
    heartBeat(-999999999)
{}

template<typename P>
Map<P>::Map(qint16 w, qint16 h, qint16 bs) :
    maxNbPlayers(0),
    NbPlayers(0),
    block_list(NULL),
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
    sigBlockChanged(pos);
}

template<typename P>
void Map<P>::setType(BlockMapProperty::BlockType type, int x, int y)
{
    block_list[y*width+x].setType(type);
    sigBlockChanged(x,y);
}

template<typename P>
void Map<P>::getBlockPosition(int x, int y, int &xdest, int &ydest) const
{
    xdest = x / blockSize;
    ydest = y / blockSize;
}

template<typename P>
void Map<P>::getNextBlock(int x, int y, int &xdest, int &ydest, int direction) const
{
    xdest = x;
    ydest = y;
    switch(direction)
    {
    case MOVE_LEFT:
        if(xdest > 0)
            xdest--;
        break;
    case MOVE_RIGHT:
        if(xdest < width - 1)
            xdest++;
        break;
    case MOVE_DOWN:
        if(ydest > 0)
            ydest--;
        break;
    case MOVE_UP:
        if(ydest < height - 1)
            ydest++;
        break;
    default:
        break;
    }
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
bool Map<P>::setPlayerSickness(int id, bool sick)
{
    if(players[id]->getIsSick() != sick)
    {
        players[id]->setIsSick(sick);
        return true;
    }
    return false;
}
template<typename P>
void Map<P>::newPlayer(int id)
{
    NbPlayers++;
    players.append(new P(id));
}

template<typename P>
void Map<P>::setMaxNbPlayers(int nb)
{
    maxNbPlayers = nb;
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
            QSet<QPoint> flamePositions = f->getFlamePositions();
            QSet<QPoint>::const_iterator it = flamePositions.constBegin();

            //remove flames
            while(it != flamePositions.constEnd())
            {
                // FIXME there could be another (and more recent) flame on the same square!
                setType(BlockMapProperty::empty, (*it).x(), (*it).y());
                it++;
            }

            //remove broken blocks
            QSet<QPoint> brokenBlocks = f->getBrokenBlocks();
            foreach(QPoint bb, brokenBlocks)
            {
                //qDebug() << "GameArena brokenblock";
                setType(BlockMapProperty::empty, bb.x(), bb.y());
                brokenBlockRemoved(bb.x(), bb.y());
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
void Map<P>::addBomb(int plId, int squareX, int squareY, int bombId, bool rc)
{
    Bomb *newBomb = new Bomb(plId, squareX, squareY, bombId, rc);
    bombs.append(newBomb);
    qDebug() << " Map> AddBomb : " << bombs.size() << " BOMBS !!! x: "<<squareX<<" y: "<<squareY<<"bombId: "<<newBomb->bombId;
}

template<typename P>
void Map<P>::removeBomb(qint16 bombId)
{
    foreach (Bomb *b, bombs)
    {
        if(b->bombId == bombId)
        {
            bombs.removeOne(b);
            delete b;
            return;
        }
    }
    qDebug() << "nothing has been removed";
}

template<typename P>
const Bomb * Map<P>::getBomb(qint16 bombId)
{
    foreach (Bomb *b, bombs)
    {
      if(b->bombId == bombId)
      {
          return b;
      }
    }
    return NULL;
}


template<typename P>
int Map<P>::blockContainsBomb(int x,int y) const
{
    foreach( Bomb *b, bombs)
    {
        if ((b->x == x) && (b->y == y))
            return b->getPlayer();
    }
    return -1;
}


template<typename P>
void Map<P>::setHeartBeat(qint32 hb) {
    heartBeat = hb;
    sigHeartbeatUpdated(hb);
}


template<typename P>
int Map<P>::coordinatePositionInBlock(int coord)
{
    int block = coord / blockSize;
    int middle = blockSize * block + blockSize/2;
    return coord - middle;
}


template<typename P>
QDataStream &operator>>(QDataStream & in, Map<P> &map) {
    qint8 NbPlayers, MaxNbPlayer;
    qint16 width, height, blockSize;
    in >> MaxNbPlayer >> NbPlayers >> width >> height >> blockSize;
    map.setDim(width, height, blockSize);
    map.setMaxNbPlayers(MaxNbPlayer);
    //retreive player position from the data stream
    qint16 x,y;
    for(int i = 0; i < NbPlayers; i++)
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
    out << map.getMaxNbPlayers() << map.getNbPlayers() << map.getWidth() << map.getHeight() << map.getBlockSize();
    //copy player positions in our data stream
    //out.writeBytes((const char *)map.getPlayersPosition(),map.getMaxNbPlayers()*sizeof(int));
    qint16 x,y;
    for(int i = 0; i < map.getNbPlayers(); i++)
    {
        map.getPlayerPosition(i,x,y);
        out << x << y;
    }
    //copy block types in our data stream
    for(int i = 0; i < map.getWidth()*map.getHeight(); i++)
        out << (qint16)map.getType(i);
    return out;
}