/*
    Copyright (C) 2010 Sébastien Escudier

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

#include "Bomb.h"
#include <QDebug>

int Bomb::index=1;
//constructor for server
Bomb::Bomb(int range, int playerId, int duration, int x, int y) :
		blinkTimer(this)
{
	qDebug() << "Bomb constructor";
	this->range = range;
    this->playerId = playerId;
    this->duration = duration;
    this->x=x;
    this->y=y;
	blinkTimer.setSingleShot(true);
	connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(bombTimeout()));
	blinkTimer.start(duration);
    this->bombId=index;
      index++;
}

//constructor for client
Bomb::Bomb(int playerId, int x, int y, int bombId)
{
   this->playerId = playerId;
   this->x=x;
   this->y=y;
   this->bombId=bombId;
}
void Bomb::bombTimeout(){

	emit explode(this);
}


Bomb::~Bomb()
{
}
//bool Bomb::operator< (const Bomb & b)
//{
//	return this->x*100+this->y<b.x*100+b.y;
//}
