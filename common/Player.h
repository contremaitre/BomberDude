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

#ifndef QTB_PLAYER_H
#define QTB_PLAYER_H


#include <QDataStream>

#include "constant.h"


class Player {
public:
    typedef qint8 playerId_t;

	Player();
	Player(qint8 playerId);

	qint8 getId() const					{ return id; }

	void setX(qint16 x)					{ this->x = x; }
	qint16 getX() const					{ return x; }
	void setY(qint16 y)					{ this->y = y; }
	qint16 getY() const					{ return y; }

    void setDirection(int dir)			{ direction = dir; }
    int getDirection() const			{ return direction; }
    void setHeading(globalDirection h)  { heading = h;}
    globalDirection getHeading() const  { return heading;}

    bool getIsAlive() const             { return isAlive; }

    bool getIsSick() const              { return isSick; }
    void setIsSick(bool sick)           { isSick = sick; }

    void setMove( bool m )              { move = m; }
    bool getMove() const                { return move; }
protected:
	qint8 id;

	qint16 x;
	qint16 y;

	int direction;
	globalDirection heading;

    bool isAlive;
    bool isSick;
    bool move; //the player moved, or tried to move, during the current heartbeat. Needed for the animation

	// allows easier serialization, avoids exposing all internal variables for writing
	friend QDataStream& operator>>(QDataStream& in, Player& f);
	friend QDataStream& operator<<(QDataStream& out, const Player& f);
};

QDataStream& operator>>(QDataStream& in, Player& f);
QDataStream& operator<<(QDataStream& out, const Player& f);


#endif // QTB_PLAYER_H
