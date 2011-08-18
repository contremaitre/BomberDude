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
        QPixmap * tmpPix= new QPixmap("pictures/bomb.png");
        QBomb::normalPix.append(tmpPix);

        tmpPix= new QPixmap("pictures/bomb2.png");
        QBomb::normalPix.append(tmpPix);
    }
    if (QBomb::rcPix.isEmpty())
    {
        QPixmap * tmpPix= new QPixmap("pictures/bomb_rc.png");
        QBomb::rcPix.append(tmpPix);
    }
    if (QBomb::dudPix.isEmpty())
    {
        QPixmap * tmpPix= new QPixmap("pictures/bomb_dud.png");
        QBomb::dudPix.append(tmpPix);
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

void QBomb::setPos(int x,int y,int size)
{
    setX(x);
    setY(y);
    this->size=size;
}

QRectF QBomb::boundingRect() const
{
    return QRectF(0,0,size,size);
}

void QBomb::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(0,0,size,size,*(currentAnim->at(currentPix)));
}



void QBomb::nextFrame()
{
    if (currentPix>=currentAnim->size()-1)
        currentPix=0;
    else
        currentPix++;
    update();
}
