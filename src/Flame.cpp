/*
 * Flame.cpp
 *
 *  Created on: 4 oct. 2010
 *      Author: lartick
 */

#include "Flame.h"
#include <QDebug>


int Flame::index=1;

void Flame::addFlame(int x, int y) {
	flames.append(new QPoint(x,y));
}

QList<QPoint*> Flame::getFlamePositions() const{
	return flames;
}
Flame::Flame() {
}

//constructor for server
Flame::Flame(int playerId, int duration) {
	this->playerId=playerId;
	flameId=index;
	index++;
    blinkTimer=new QTimer(this);
	blinkTimer->setSingleShot(true);
	connect(blinkTimer, SIGNAL(timeout()), this, SLOT(flameTimeout()));
	//blinkTimer->start(duration);
	this->duration=duration;
	qDebug()<< "connect timer to flameTimeOut "<<duration;//<< " "<<blinkTimer->isActive();

}

void Flame::startFlameTimer(){
	blinkTimer->start(duration);
}

Flame::~Flame() {
	// TODO Auto-generated destructor stub
}


void Flame::flameTimeout()
{
	qDebug() << "Flame>flameTimeout";
	emit flameEnd(*this);
}


int Flame::getFlameId() const{
	return flameId;
}

void Flame::setFlameId(int id){
	flameId=id;
}

QDataStream &operator<<(QDataStream &out, const Flame &flame)
{
    out << (qint16)(flame.getFlamePositions().size()) << (qint16) flame.getFlameId();
    foreach(QPoint* flamePosition , flame.getFlamePositions())
    {
    	out << (qint16)flamePosition->x() << (qint16)flamePosition->y();
    }
    return out;
}

QDataStream &operator>>(QDataStream & in, Flame &flame)
{
    qint16 numberOfFlames, flameId;
    in >> numberOfFlames >> flameId;
    flame.setFlameId(flameId);
    for (int i=0;i<numberOfFlames;i++)
    {
    	qint16 x,y;
    	in >> x >> y;
    	flame.addFlame(x,y);
    }
    return in;
}
