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


class Bomb
{
public:
	Bomb();
	Bomb(qint8 playerId, qint16 x, qint16 y, int duration, int range, bool remote, bool oil);
	Bomb(qint8 playerId, qint16 x, qint16 y, qint16 bombId, bool remote);
	~Bomb();

	void decreaseLifeSpan()					{ if(!remoteControlled) duration--; }

	bool mustExplode()						{ return !remoteControlled && duration < 0; }
    qint8 getPlayer()                       { return playerId; }

    qint16 getX() const                     { return x; }
    qint16 getY() const                     { return y; }

    void setX(qint16 val)                   { x = val; }
    void setY(qint16 val)                   { y = val; }

private:
	static qint16 index;

    qint16 x;
    qint16 y;

public:
    //owner
    qint8 playerId;
	//duration in heartbeats
    int duration;
    int range;
    qint16 bombId;
    bool remoteControlled;
    bool hasOil;
    globalDirection direction;

	friend QDataStream& operator>>(QDataStream& in, Bomb& f);
	friend QDataStream& operator<<(QDataStream& out, const Bomb& f);
};

QDataStream& operator>>(QDataStream& in, Bomb& f);
QDataStream& operator<<(QDataStream& out, const Bomb& f);


#endif // QTB_BOMB_H
