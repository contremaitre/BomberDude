#ifndef QTB_PLAYER_H
#define QTB_PLAYER_H


#include <QDataStream>


class Player {
public:
	Player();
	Player(qint8 playerId);

	void setId(qint8 id)				{ this->id = id; }
	qint8 getId() const					{ return id; }

	void setX(qint16 x)					{ this->x = x; }
	qint16 getX() const					{ return x; }
	void setY(qint16 y)					{ this->y = y; }
	qint16 getY() const					{ return y; }

	void setDirection(int dir)			{ latestDirection = dir; }
	int getDirection() const			{ return latestDirection; }

	void raiseLayingBomb()				{ layingBomb = true; }
	void clearLayingBomb()				{ layingBomb = false; }
	bool getLayingBomb() const			{ return layingBomb; }

private:
	qint8 id;

	qint16 x;
	qint16 y;

	int latestDirection;
	bool layingBomb;

	// allows easier serialization, avoids exposing all internal variables for writing
	friend QDataStream& operator>>(QDataStream& in, Player& f);
	friend QDataStream& operator<<(QDataStream& out, const Player& f);
};

QDataStream& operator>>(QDataStream& in, Player& f);
QDataStream& operator<<(QDataStream& out, const Player& f);


#endif // QTB_PLAYER_H
