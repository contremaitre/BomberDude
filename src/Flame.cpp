/*
    Copyright (C) 2010 lartick

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

#include "Flame.h"
#include <QDebug>


int Flame::index=1;

void Flame::addFlame(int x, int y) {
	flames.append(QPoint(x,y));
}

QList<QPoint> Flame::getFlamePositions() const{
	return flames;
}
Flame::Flame() {
}

//constructor for server
Flame::Flame(int playerId, int duration) :
		blinkTimer(this)
{
	this->playerId=playerId;
	flameId=index;
	index++;
	blinkTimer.setSingleShot(true);
	connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(flameTimeout()));
	//blinkTimer.start(duration);
	this->duration=duration;
	//qDebug()<< "connect timer to flameTimeOut "<<duration;//<< " "<<blinkTimer.isActive();

}

void Flame::startFlameTimer(){
	blinkTimer.start(duration);
}

Flame::~Flame() {
	// TODO Auto-generated destructor stub
}


void Flame::flameTimeout()
{
	//qDebug() << "Flame>flameTimeout";
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
	foreach(const QPoint flamePosition , flame.getFlamePositions())
    {
		out << (qint16)flamePosition.x() << (qint16)flamePosition.y();
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
