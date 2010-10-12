#ifndef PLAYER_H
#define PLAYER_H


#include <QDataStream>


class Player {
public:
	Player();
	Player(int playerId);

	void setId(int id)			{ this->id = id; }
	int getId() const			{ return id; }

	void setX(int x)					{ this->x = x; }
	int getX() const					{ return x; }
	void setY(int y)					{ this->y = y; }
	int getY() const					{ return y; }

	void setDirection(int dir)			{ latestDirection = dir; }
	int getDirection() const			{ return latestDirection; }

	void raiseLayingBomb()				{ layingBomb = true; }
	void clearLayingBomb()				{ layingBomb = false; }
	bool getLayingBomb() const			{ return layingBomb; }

private:
	int id;

	int x;
	int y;

	int latestDirection;
	bool layingBomb;

	// allows easier serialization, avoids exposing all internal variables for writing
	friend QDataStream& operator>>(QDataStream& in, Player& f);
	friend QDataStream& operator>>(QDataStream& in, Player* f);
	friend QDataStream& operator<<(QDataStream& out, const Player& f);
};

QDataStream& operator>>(QDataStream& in, Player& f);
QDataStream& operator>>(QDataStream& in, Player* f);
QDataStream& operator<<(QDataStream& out, const Player& f);


#endif // PLAYER_H
