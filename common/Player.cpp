#include "Player.h"
#include <QDebug>


Player::Player() :
	id(-1),
	x(-1),
	y(-1),
	latestDirection(-1),
    isAlive(true),
    currentSickness(SICK_NONE)
{}

Player::Player(qint8 playerId) :
	id(playerId),
	x(-1),
	y(-1),
	latestDirection(-1),
    isAlive(true),
    currentSickness(SICK_NONE)
{}


enum bitFieldIndex { BFI_IS_ALIVE,
                      BFI_SICKNESS,
                    };

QDataStream& operator>>(QDataStream& in, Player& p) {
	in >> p.id >> p.x >> p.y;

    quint8 bitField;
    in >> bitField;
    p.isAlive = bitField & (1 << BFI_IS_ALIVE);
    p.currentSickness = (bitField & (1 <<BFI_SICKNESS)) ? SICK_UNKNOWN : SICK_NONE;

	return in;
}

QDataStream& operator<<(QDataStream& out, const Player& p) {
	out << p.id << p.x << p.y;

    quint8 bitField = 0;
    if(p.isAlive)
        bitField |= 1 << BFI_IS_ALIVE;
    if(p.currentSickness != SICK_NONE)
        bitField |= 1 << BFI_SICKNESS;
    out << bitField;

	return out;
}
