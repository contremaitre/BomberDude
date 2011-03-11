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

#include "Bomb.h"
#include <QDebug>

qint16 Bomb::index=1;

//constructor for serialization
Bomb::Bomb() :
	x(-1),
	y(-1),
	playerId(-1),
	duration(-1),
	range(-1),
	bombId(-1),
	remoteControlled(false),
    hasOil(false),
    direction(dirNone)
{}

//constructor for server
Bomb::Bomb(qint8 playerId, qint16 x, qint16 y, int duration, int range, bool remote, bool oil)
{
    this->range = range;
    this->playerId = playerId;
    this->duration = duration;
    this->x = x;
    this->y = y;
    this->bombId = index;
    this->remoteControlled = remote;
    this->hasOil = oil;
    this->direction = dirNone;
    index++;
}

//constructor for client
Bomb::Bomb(qint8 playerId, qint16 x, qint16 y, qint16 bombId, bool remote)
{
    this->playerId = playerId;
    this->x = x;
    this->y = y;
    this->bombId = bombId;
    this->remoteControlled = remote;
    range = duration = -1;
    hasOil = false;
    direction = dirNone;
}

Bomb::~Bomb()
{
}

QDataStream& operator>>(QDataStream& in, Bomb& f)
{
    qint8 rc;
    in >> f.bombId >> f.x >> f.y >> f.playerId;
    in >> rc;
    f.remoteControlled = rc != 0;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Bomb& f)
{
    out << f.bombId << f.x << f.y << f.playerId << static_cast<qint8>(f.remoteControlled);
    return out;
}
