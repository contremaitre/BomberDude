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

QBomb::QBomb()
{
    size=0;
    currentPix=0;
    initPixList();
}
void QBomb::initPixList()
{
    QPixmap * tmpPix= new QPixmap("pictures/bomb.png");
    tmpPix->scaled(QSizeF(size,size).toSize());
    pixmaps.append(tmpPix);

    tmpPix= new QPixmap("pictures/bomb2.png");
    tmpPix->scaled(QSizeF(size,size).toSize());
    pixmaps.append(tmpPix);
/*
    tmpPix= new QPixmap("pictures/bomb3.png");
    tmpPix->scaled(QSizeF(size,size).toSize());
    pixmaps.append(tmpPix);

    tmpPix= new QPixmap("pictures/bomb4.png");
    tmpPix->scaled(QSizeF(size,size).toSize());
    pixmaps.append(tmpPix);
*/
}

QBomb::QBomb(int x, int y , int size)
{
    currentPix=0;
    setPos(x,y,size);
    initPixList();

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
    painter->drawPixmap(0,0,size,size,*(pixmaps.at(currentPix)));
}



void QBomb::nextFrame()
{
    if (currentPix>=pixmaps.size()-1)
        currentPix=0;
    else
        currentPix++;
    update();
}
