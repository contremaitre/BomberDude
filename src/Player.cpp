#include "Player.h"
#include <QDebug>

Player::Player() :
	id(-1),
	x(-1),
	y(-1),
	latestDirection(-1),
	layingBomb(false)
{}

Player::Player(qint8 playerId) :
	id(playerId),
	x(-1),
	y(-1),
	latestDirection(-1),
	layingBomb(false)
{}

QDataStream& operator>>(QDataStream& in, Player& p) {
	in >> p.id >> p.x >> p.y;
	return in;
}

QDataStream& operator<<(QDataStream& out, const Player& p) {
	out << p.id << p.x << p.y;
	return out;
}
