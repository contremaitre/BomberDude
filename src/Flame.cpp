/*
 * Flame.cpp
 *
 *  Created on: 4 oct. 2010
 *      Author: lartick
 */

#include "Flame.h"

void Flame::addFlame(int x, int y) {
	flames.append(new QPoint(x,y));
}

QList<QPoint*> Flame::getFlamePositions() const{
	return flames;
}
Flame::Flame() {
}

Flame::Flame(int playerId, int duration) {
	this->playerId=playerId;
	//this->blinkTimer=new QTimer(this);
}

Flame::~Flame() {
	// TODO Auto-generated destructor stub
}

QDataStream &operator<<(QDataStream &out, const Flame &flame)
{
    out << (qint16)(flame.getFlamePositions().size()) ;
    foreach(QPoint* flamePosition , flame.getFlamePositions())
    {
    	out << (qint16)flamePosition->x() << (qint16)flamePosition->y();
    }
    return out;
}

QDataStream &operator>>(QDataStream & in, Flame &flame)
{
    qint16 numberOfFlames;
    in >> numberOfFlames;

    for (int i=0;i<numberOfFlames;i++)
    {
    	qint16 x,y;
    	in >> x >> y;
    	flame.addFlame(x,y);
    }
    return in;
}
