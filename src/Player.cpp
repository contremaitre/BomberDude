#include "Player.h"


Player::Player(int playerId) :
	id(playerId),
	x(-1),
	y(-1),
	latestDirection(-1)
{}

QDataStream &operator>>(QDataStream & in, Player &p) {
	in >> p.id >> p.x >> p.y;
	return in;
}

QDataStream &operator<<(QDataStream &out, const Player &p) {
	out << p.id << p.x << p.y;
	return out;
}
