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
	~Bomb();

protected:
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
    // TODO stop sending playerId to client? Seems useless
    qint8 playerId;                     /// owner of the bomb

    qint16 x;
    qint16 y;

    bool remoteControlled;

public:
	friend QDataStream& operator>>(QDataStream& in, Bomb& f);
	friend QDataStream& operator<<(QDataStream& out, const Bomb& f);
};

QDataStream& operator>>(QDataStream& in, Bomb& f);
QDataStream& operator<<(QDataStream& out, const Bomb& f);


#endif // QTB_BOMB_H
