/*
    Copyright (C) 2011,2012 Sébastien Escudier

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


QBomb::QBomb() :
        flying(false), xStep(0), yStep(0), nbStep(0), normalSize(0)
{
    Q_ASSERT(!normalPix.empty());
}

QBomb::QBomb(int x, int y, int size) :
        flying(false), xStep(0), yStep(0), nbStep(0), normalSize(0)
{
    Q_ASSERT(!normalPix.empty());
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

void QBomb::setFlying(int nb, int x, int y)
{
    if(!flying)
        normalSize = size;
    flying = true;
    nbStep = nb;
    xStep = x/nb;
    yStep = y/nb;
}

void QBomb::setNonFlying()
{
    if(flying)
    {
        flying = false;
        setPos(x(), y(), normalSize);
    }
}

void QBomb::nextFrame(int hb)
{
    if(flying && --nbStep > 0)
    {
        //setPos(x() - xStep, y() - yStep, normalSize);
        int newSize = size;
        if( hb % 5 == 0 )
            newSize = size > normalSize ? normalSize : normalSize + 10;
        setPos(x() - xStep, y() - yStep, newSize);
    }
    QAnimatedItem::nextFrame();
}


void QBomb::loadPixs()
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

void QBomb::unloadPixs()
{
    while(!normalPix.empty())
        delete normalPix.takeFirst();
    while(!rcPix.empty())
        delete rcPix.takeFirst();
    while(!dudPix.empty())
        delete dudPix.takeFirst();
}

QBomb::~QBomb()
{
}
