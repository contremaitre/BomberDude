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
#include "qbomb.h"

QList<QPixmap*> QBomb::normalPix= QList<QPixmap*>();
QList<QPixmap*> QBomb::rcPix= QList<QPixmap*>();
QList<QPixmap*> QBomb::dudPix= QList<QPixmap*>();


QBomb::QBomb()
{
    initPixList();
    size=0;
    currentPix=0;
}
void QBomb::initPixList()
{
    if (QBomb::normalPix.isEmpty())
    {
        QAnimatedItem::appendNewFrame(&QBomb::normalPix, "pictures/bomb.png");
        QAnimatedItem::appendNewFrame(&QBomb::normalPix, "pictures/bomb2.png");
    }
    if (QBomb::rcPix.isEmpty())
    {
        QAnimatedItem::appendNewFrame(&QBomb::rcPix, "pictures/bomb_rc.png");
        QAnimatedItem::appendNewFrame(&QBomb::rcPix, "pictures/bomb_rc2.png");
    }
    if (QBomb::dudPix.isEmpty())
    {
        QAnimatedItem::appendNewFrame(&QBomb::dudPix, "pictures/bomb_dud.png");
        QAnimatedItem::appendNewFrame(&QBomb::dudPix, "pictures/bomb2.png");
    }
}

QBomb::QBomb(int x, int y , int size)
{
    initPixList();
    currentPix=0;
    currentAnim=&QBomb::normalPix;
    setPos(x,y,size);

}
void QBomb::setNormalBomb()
{
    currentPix=0;
    currentAnim=&QBomb::normalPix;
}
void QBomb::setDudBomb()
{
    currentPix=0;
    currentAnim=&QBomb::dudPix;
}
void QBomb::setRC()
{
    currentPix=0;
    currentAnim=&QBomb::rcPix;
}





