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


int Flame::index=1;


uint qHash(const QPoint& key) {
	return qHash((key.x() << 8) + key.y());
}


Flame::Flame() {
}

//constructor for server
Flame::Flame(int playerId, int duration)
{
	this->playerId=playerId;
	flameId=index;
	index++;
	this->duration=duration;
}

Flame::~Flame() {
	// TODO Auto-generated destructor stub
}

void Flame::addFlame(int x, int y) {
	flames.insert(QPoint(x,y));
}

void Flame::addBrokenBlock(int x, int y) {
	brokenBlocks.insert(QPoint(x,y));
}

QDataStream &operator<<(QDataStream &out, const Flame& flame)
{
	out << flame.flameId;
	out << flame.flames;
	out << flame.brokenBlocks;

	return out;
}

QDataStream &operator>>(QDataStream & in, Flame& flame)
{
	in >> flame.flameId;
	in >> flame.flames;
	in >> flame.brokenBlocks;

	return in;
}

QDataStream &operator<<(QDataStream &out, const Flame* flame)
{
	out << flame->flameId;
	out << flame->flames;
	out << flame->brokenBlocks;

	return out;
}

QDataStream &operator>>(QDataStream & in, Flame* flame)
{
	in >> flame->flameId;
	in >> flame->flames;
	in >> flame->brokenBlocks;

    return in;
}
