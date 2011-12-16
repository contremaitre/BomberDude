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
