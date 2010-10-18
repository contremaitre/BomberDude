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
	bombId(-1)
{}

//constructor for server
Bomb::Bomb(int range, qint8 playerId, int duration, qint16 x, qint16 y)
{
	qDebug() << "Bomb constructor";
	this->range = range;
    this->playerId = playerId;
    this->duration = duration;
    this->x=x;
    this->y=y;
    this->bombId=index;
      index++;
}

//constructor for client
Bomb::Bomb(qint8 playerId, qint16 x, qint16 y, qint16 bombId)
{
   this->playerId = playerId;
   this->x=x;
   this->y=y;
   this->bombId=bombId;
}

Bomb::~Bomb()
{
}

QDataStream& operator>>(QDataStream& in, Bomb& f) {
	in >> f.bombId >> f.x >> f.y >> f.playerId;
	return in;
}

QDataStream& operator<<(QDataStream& out, const Bomb& f) {
	out << f.bombId << f.x << f.y << f.playerId;
	return out;
}
