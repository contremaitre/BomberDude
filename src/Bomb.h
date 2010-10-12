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

#ifndef BOMB_H
#define BOMB_H

#include <QObject>
#include <QTimer>


class Bomb : public QObject
{
    Q_OBJECT

public:
	Bomb();
	Bomb(int range, int playerId, int duration, int x, int y);
	Bomb(int playerId, int x, int y,int bombId);
	~Bomb();

	void decreaseLifeSpan()					{ duration--; }

	// TODO must also check that the bomb is not in movement
	bool mustExplode()						{ return duration < 0; }

private:
	static int index;

public:
    int x;
    int y;
    //owner
    int playerId;
	//duration in heartbeats
    int duration;
    int range;
    int bombId;

	friend QDataStream& operator>>(QDataStream& in, Bomb& f);
	friend QDataStream& operator<<(QDataStream& out, const Bomb& f);
};

QDataStream& operator>>(QDataStream& in, Bomb& f);
QDataStream& operator<<(QDataStream& out, const Bomb& f);


#endif
