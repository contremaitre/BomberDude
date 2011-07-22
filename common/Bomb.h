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

#ifndef QTB_BOMB_H
#define QTB_BOMB_H


#include <QDataStream>
#include <QPoint>
#include "constant.h"

template<typename TileComp>
class Bomb : public QObject
{
public:
    typedef qint16 bombId_t;

	virtual ~Bomb();

protected:
	Bomb(TileComp tc);

    Bomb(qint16 bombId,
         qint8 playerId,
         qint16 x,
         qint16 y,
         bool remoteControlled,
         TileComp tc);

public:
    qint16 getBombId() const                { return bombId; }
    qint8 getPlayerId() const               { return playerId; }

    qint16 getX() const                     { return x; }
    qint16 getY() const                     { return y; }

    void setX(qint16 val);
    void setY(qint16 val);

    qint8 getTileX() const                  { return tx; }
    qint8 getTileY() const                  { return ty; }

    bool getIsRC() const                    { return remoteControlled; }
    void unsetRC()                          { remoteControlled = false; }

    bool getDudBomb() const                 { return dud_bomb; }
    void setFlying(bool f);
    bool getFlying() const                  { return flying; }
    void setDestination(const QPoint& block){ fl_destination = block; }
    QPoint getDestination() const           { return fl_destination; }
    void setFlHeartbeat(qint32 h)           { fl_Heartbeat = h; }
    qint32 getFlHeartbeat() const           { return fl_Heartbeat; }

private:
    qint16 bombId;
    qint8 playerId;                     /// owner of the bomb

    // coordinates in pixel, rw
    qint16 x;
    qint16 y;

    // coordinates in tiles, ro
    qint8 tx;
    qint8 ty;

    bool remoteControlled;

    TileComp functorToTiles;            /// this functor will convert pixel coordinate to tile coordinate

protected:
    bool dud_bomb;
    bool flying;                            /// When a player use a glove, the bomb is flying above the blocks
    QPoint fl_destination;                  /// If a bomb if flying indicate its destination
    qint32 fl_Heartbeat;                       /// Indicate at which heartbeat the bomb will arrive

public:
    virtual void sigTileChanged(qint16 bombId, qint8 oldx, qint8 oldy, qint8 newx, qint8 newy) = 0;
    virtual void sigFlyingBombChange(qint16 bombId) = 0;

    template<typename T>
	friend QDataStream& operator>>(QDataStream& in, Bomb<T>& f);

    template<typename T>
	friend QDataStream& operator<<(QDataStream& out, const Bomb<T>& f);
};


template<typename TileComp>
Bomb<TileComp>::Bomb(TileComp tc) :
	bombId(-1),
	playerId(-1),
	x(-1),
	y(-1),
    tx(-1),
    ty(-1),
    remoteControlled(false),
    functorToTiles(tc),
    dud_bomb(false),
    flying(false),
    fl_Heartbeat(0)
{}


template<typename TileComp>
Bomb<TileComp>::Bomb(qint16 bombId,
                     qint8 playerId,
                     qint16 x,
                     qint16 y,
                     bool remoteControlled,
                     TileComp tc) :
    bombId(bombId),
    playerId(playerId),
    x(x),
    y(y),
    remoteControlled(remoteControlled),
    functorToTiles(tc),
    dud_bomb(false),
    flying(false),
    fl_Heartbeat(0)
{
    tx = functorToTiles(x);
    ty = functorToTiles(y);
}


template<typename TileComp>
Bomb<TileComp>::~Bomb()
{}

template<typename TileComp>
void Bomb<TileComp>::setFlying( bool f )
{
    flying = f;
    emit sigFlyingBombChange(bombId);
}

template<typename TileComp>
void Bomb<TileComp>::setX(qint16 val)
{
    x = val;
    qint8 ntx = functorToTiles(val);
    if(ntx != tx) {
        if(!flying && tx != -1) //we must not emit this signal if the bomb is flying because it is not attached to the tile
            emit sigTileChanged(bombId, tx, ty, ntx, ty);
        tx = ntx;
    }
}


template<typename TileComp>
void Bomb<TileComp>::setY(qint16 val)
{
    y = val;
    qint8 nty = functorToTiles(val);
    if(nty != ty) {
        if(!flying && ty != -1) //we must not emit this signal if the bomb is flying because it is not attached to the tile
            emit sigTileChanged(bombId, tx, ty, tx, nty);
        ty = nty;
    }
}


template<typename TileComp>
QDataStream& operator>>(QDataStream& in, Bomb<TileComp>& f)
{
    qint8 tmp;
    in >> f.bombId >> f.x >> f.y >> f.playerId;
    in >> tmp;
    f.remoteControlled = tmp != 0;
    in >> tmp;
    f.dud_bomb = tmp != 0;
    return in;
}


template<typename TileComp>
QDataStream& operator<<(QDataStream& out, const Bomb<TileComp>& f)
{
    out << f.bombId << f.x << f.y << f.playerId << static_cast<qint8>(f.remoteControlled)
        << static_cast<qint8>(f.dud_bomb);
    return out;
}


#endif // QTB_BOMB_H
