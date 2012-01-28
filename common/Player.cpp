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

#include "Player.h"
#include <QDebug>


Player::Player() :
	id(-1),
	x(-1),
	y(-1),
    direction(-1),
    heading(dirNone),
    isAlive(false),
    isSick(false),
    move(false)
{}

Player::Player(qint8 playerId) :
	id(playerId),
	x(-1),
	y(-1),
    direction(-1),
    heading(dirNone),
    isAlive(false),
    isSick(false),
    move(false)
{}


enum bitFieldIndex { BFI_IS_ALIVE,
                      BFI_SICKNESS,
                      BFI_MOVED, //player moved (or tried to)
                    };

QDataStream& operator>>(QDataStream& in, Player& p) {
	in >> p.id >> p.x >> p.y;

	qint8 h;
	in >> h;
	p.heading = static_cast<globalDirection>(h);

    quint8 bitField;
    in >> bitField;
    p.isAlive = bitField & (1 << BFI_IS_ALIVE);
    p.isSick = (bitField & (1 <<BFI_SICKNESS));
    p.move =  (bitField & (1 <<BFI_MOVED));
	return in;
}

QDataStream& operator<<(QDataStream& out, const Player& p) {
	out << p.id << p.x << p.y << static_cast<qint8>(p.heading);

    quint8 bitField = 0;
    if(p.isAlive)
        bitField |= 1 << BFI_IS_ALIVE;
    if(p.isSick)
        bitField |= 1 << BFI_SICKNESS;
    if(p.move)
        bitField |= 1 << BFI_MOVED;
    out << bitField;

	return out;
}
