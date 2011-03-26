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

#ifndef QTB_BOMB_H
#define QTB_BOMB_H


#include <QDataStream>

#include "constant.h"


template<typename TileComp>
class Bomb : public QObject
{
public:
    typedef qint16 bombId_t;

	virtual ~Bomb();

protected:
	Bomb();
    Bomb(qint16 bombId, qint8 playerId, bool remoteControlled);

public:    
    qint16 getBombId() const                { return bombId; }
    qint8 getPlayerId() const               { return playerId; }

    qint16 getX() const                     { return x; }
    qint16 getY() const                     { return y; }

    void setX(qint16 val)                   { x = val; }
    void setY(qint16 val)                   { y = val; }

    bool getIsRC() const                    { return remoteControlled; }
    void unsetRC()                          { remoteControlled = false; }

private:
    qint16 bombId;
    qint8 playerId;                     /// owner of the bomb

    qint16 x;
    qint16 y;

    bool remoteControlled;

public:
    virtual void sigTileChanged() = 0;

    template<typename T>
	friend QDataStream& operator>>(QDataStream& in, Bomb<T>& f);

    template<typename T>
	friend QDataStream& operator<<(QDataStream& out, const Bomb<T>& f);
};


template<typename TileComp>
Bomb<TileComp>::Bomb() :
	bombId(-1),
	playerId(-1),
	x(-1),
	y(-1),
	remoteControlled(false)
{}


template<typename TileComp>
Bomb<TileComp>::Bomb(qint16 bombId, qint8 playerId, bool remoteControlled) :
    bombId(bombId),
    playerId(playerId),
    remoteControlled(remoteControlled)
{}


template<typename TileComp>
Bomb<TileComp>::~Bomb()
{}


template<typename TileComp>
QDataStream& operator>>(QDataStream& in, Bomb<TileComp>& f)
{
    qint8 rc;
    in >> f.bombId >> f.x >> f.y >> f.playerId;
    in >> rc;
    f.remoteControlled = rc != 0;
    return in;
}


template<typename TileComp>
QDataStream& operator<<(QDataStream& out, const Bomb<TileComp>& f)
{
    out << f.bombId << f.x << f.y << f.playerId << static_cast<qint8>(f.remoteControlled);
    return out;
}


#endif // QTB_BOMB_H
