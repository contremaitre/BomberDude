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

template<typename P, typename SL>
Map<P,SL>::Map() :
    NbPlayers(0),
    maxNbPlayers(0),
    width(0),
    height(0),
    block_list(NULL),
    blockSize(0),
    heartBeat(-999999999)
{}

template<typename P, typename SL>
Map<P,SL>::Map(qint16 w, qint16 h, qint16 bs) :
    maxNbPlayers(0),
    NbPlayers(0),
    block_list(NULL),
    heartBeat(-999999999)
{
    setDim(w,h,bs);
}

template<typename P, typename SL>
Map<P,SL>::~Map()
{
    delete[] block_list;

    foreach(P* p, players)
        delete p;

    foreach(Bomb* b, bombs)
        delete b;

    foreach(Flame* f, flames)
        delete f;
}

template<typename P, typename SL>
void Map<P,SL>::setDim(qint16 w, qint16 h, qint16 block_size)
{
    width = w;
    height = h;
    blockSize = block_size;
    Init();
}

template<typename P, typename SL>
void Map<P,SL>::Init()
{
    if( width * height <= 0 )
        return;

    delete[] block_list;
    block_list = new BlockMapProperty[width*height];

    qDebug() << "init";
}

template<typename P, typename SL>
void Map<P,SL>::setType(BlockMapProperty::BlockType type, int pos)
{
    block_list[pos].setType(type);
    sigBlockChanged(pos);
}

template<typename P, typename SL>
void Map<P,SL>::setType(BlockMapProperty::BlockType type, int x, int y)
{
    block_list[y*width+x].setType(type);
    sigBlockChanged(x,y);
}

template<typename P, typename SL>
void Map<P,SL>::setOption(int pos, BlockMapProperty::BlockOption option, globalDirection dir)
{
    block_list[pos].setOption(option, dir);
}

template<typename P, typename SL>
void Map<P,SL>::setOption(int x, int y, BlockMapProperty::BlockOption option, globalDirection dir)
{
    block_list[y*width+x].setOption(option, dir);
}

template<typename P, typename SL>
void Map<P,SL>::getBlockPosition(int x, int y, int &xdest, int &ydest) const
{
    xdest = x / blockSize;
    ydest = y / blockSize;
}

template<typename P, typename SL>
QPoint Map<P,SL>::getCenterCoordForBlock(int x, int y) const
{
    return QPoint(x * blockSize + blockSize/2, y * blockSize + blockSize/2);
}

template<typename P, typename SL>
QPoint Map<P,SL>::getOverlappingBlockPosition(int x, int y) const
{
    int bx = x / blockSize;
    int by = y / blockSize;

    int xc = bx * blockSize + blockSize/2;
    int yc = by * blockSize + blockSize/2;

    if(xc < x)
        bx++;
    else if(xc > x)
        bx--;
    else if(yc < y)
        by++;
    else if(yc > y)
        by--;

    return QPoint(bx,by);
}

template<typename P, typename SL>
void Map<P,SL>::getNextBlock(int x, int y, int &xdest, int &ydest, globalDirection direction) const
{
    xdest = x;
    ydest = y;
    switch(direction)
    {
        case dirLeft:
            if(xdest > 0)
                xdest--;
            break;
        case dirRight:
            if(xdest < width - 1)
                xdest++;
            break;
        case dirDown:
            if(ydest > 0)
                ydest--;
            break;
        case dirUp:
            if(ydest < height - 1)
                ydest++;
            break;
        default:
            break;
    }
}

template<typename P, typename SL>
const QList<SL> *Map<P,SL>::getStylesList() const
{
    return &styles;
}

template<typename P, typename SL>
void Map<P,SL>::getPlayerPosition(int pl, qint16 &x, qint16 &y) const
{
    x = players[pl]->getX();
    y = players[pl]->getY();
}

template<typename P, typename SL>
void Map<P,SL>::setPlayerPosition(int id, qint16 x, qint16 y)
{
    players[id]->setX(x);
    players[id]->setY(y);
}

template<typename P, typename SL>
bool Map<P,SL>::setPlayerSickness(int id, bool sick)
{
    if(players[id]->getIsSick() != sick)
    {
        players[id]->setIsSick(sick);
        return true;
    }
    return false;
}
template<typename P, typename SL>
void Map<P,SL>::newPlayer(int id)
{
    NbPlayers++;
    players.append(new P(id));
}

template<typename P, typename SL>
void Map<P,SL>::setMaxNbPlayers(int nb)
{
    maxNbPlayers = nb;
}

template<typename P, typename SL>
void Map<P,SL>::addFlame(Flame* flame)
{
    flames.append(flame);
    foreach (QPoint point, flame->getFlamePositions())
    {
        setType(BlockMapProperty::flame,point.x(),point.y());
    }
}

template<typename P, typename SL>
void Map<P,SL>::removeFlame(int flameId)
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


template<typename P, typename SL>
void Map<P,SL>::addBomb(int plId, int squareX, int squareY, int bombId, bool rc)
{
    Bomb *newBomb = new Bomb(plId, squareX, squareY, bombId, rc);
    bombs.append(newBomb);
    qDebug() << " Map> AddBomb : " << bombs.size() << " BOMBS !!! x: "<<squareX<<" y: "<<squareY<<"bombId: "<<newBomb->bombId;
}

template<typename P, typename SL>
void Map<P,SL>::removeBomb(qint16 bombId)
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

template<typename P, typename SL>
const Bomb * Map<P,SL>::getBomb(qint16 bombId)
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


template<typename P, typename SL>
Bomb *Map<P,SL>::blockContainsBomb(int x,int y) const
{
    foreach( Bomb *b, bombs)
    {
        int tx, ty;
        getBlockPosition(b->x, b->y, tx, ty);
        if(tx == x && ty == y)
            return b;
    }
    return NULL;
}


template<typename P, typename SL>
void Map<P,SL>::setHeartBeat(qint32 hb) {
    heartBeat = hb;
    sigHeartbeatUpdated(hb);
}


template<typename P, typename SL>
int Map<P,SL>::coordinatePositionInBlock(int coord)
{
    int block = coord / blockSize;
    int middle = blockSize * block + blockSize/2;
    return coord - middle;
}


template<typename P, typename SL>
QDataStream &operator>>(QDataStream & in, Map<P,SL> &map) {
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
        in >> x;
        in >> y;
        map.setOption(i,(BlockMapProperty::BlockOption)x,(globalDirection)y);
    }

    //copy style list
    quint8 listSize;
    in >> listSize;
    SL ms;
    for(int i = 0; i < listSize; i++)
    {
        in >> ms;
        map.addStyle(ms);
    }

    return in;
}

template<typename P, typename SL>
QDataStream &operator<<(QDataStream &out, const Map<P,SL> &map) {
    out << map.getMaxNbPlayers() << map.getNbPlayers() << map.getWidth() << map.getHeight() << map.getBlockSize();
    //copy player positions in our data stream
    //out.writeBytes((const char *)map.getPlayersPosition(),map.getMaxNbPlayers()*sizeof(int));
    qint16 x,y;
    for(int i = 0; i < map.getNbPlayers(); i++)
    {
        map.getPlayerPosition(i,x,y);
        out << x << y;
    }
    //copy block types and option in our data stream
    for(int i = 0; i < map.getWidth()*map.getHeight(); i++)
    {
        out << (qint16)map.getType(i);
        out << (qint16)map.getOption(i);
        out << (qint16)map.getOptionDirection(i);
    }

    //copy style list
    const QList<SL> *styleList = map.getStylesList();
    out << static_cast<quint8> (styleList->size());
    foreach(SL sl, *styleList)
        out << sl;
    return out;
}
