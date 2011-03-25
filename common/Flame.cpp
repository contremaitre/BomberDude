/*
    Copyright (C) 2010 lartick

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

#include "Flame.h"
#include <QDebug>


qint16 Flame::index=1;


uint qHash(const QPoint& key) {
	return qHash((key.x() << 8) + key.y());
}


Flame::Flame() {
}

//constructor for server
Flame::Flame(qint8 playerId, qint32 timestamp, int duration) :
    playerId(playerId),
    timestamp(timestamp),
    duration(duration)
{
	flameId = index++;
}

QDataStream &operator<<(QDataStream &out, const Flame& flame)
{
	out << flame.flameId;
	out << flame.flames;
	out << flame.brokenBlocks;

	qint16 nbBombs = flame.detonatedBombs.size();
	out << nbBombs;
	foreach(qint16 bombId, flame.detonatedBombs)
		out << bombId;

	return out;
}

QDataStream &operator>>(QDataStream & in, Flame& flame)
{
	in >> flame.flameId;
	in >> flame.flames;
	in >> flame.brokenBlocks;

	qint16 nbBombs;
	in >> nbBombs;
	for(qint16 i = 0; i < nbBombs; i++) {
		qint16 bombId;
		in >> bombId;
		flame.detonatedBombs.append(bombId);
	}

	return in;
}
