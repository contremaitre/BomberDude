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
    if (pixmaps.size()==0)
    {
        initPixList();
    }

    x=0;
    y=0;
    size=0;
    currentPix=0;
}
void QBomb::initPixList()
{
    QPixmap * tmpPix= new QPixmap("pictures/bomb.png");
    tmpPix->scaled(QSizeF(size,size).toSize());
    pixmaps.append(tmpPix);
    tmpPix=&(QPixmap("pictures/bomb2.png").scaled(QSizeF(size,size).toSize()));
    pixmaps.append(tmpPix);
    tmpPix=&(QPixmap("pictures/bomb3.png").scaled(QSizeF(size,size).toSize()));
    pixmaps.append(tmpPix);

    qDebug()<<"test"<<pixmaps.at(0)<<pixmaps.at(0)->height();
    qDebug()<<"size"<<size;
}

QBomb::QBomb(int x, int y , int size)
{
    currentPix=0;
    setPos(x,y,size);
    bRect.setX(x);
    bRect.setY(y);
    bRect.setHeight(size);
    bRect.setWidth(size);
    initPixList();

}

void QBomb::setPos(int x,int y,int size)
{
    this->x=x;
    this->y=y;
    this->size=size;
    bRect.setX(x);
    bRect.setY(y);
    bRect.setHeight(size);
    bRect.setWidth(size);
    //pix.setOffset(QPointF(x,y));
}

void QBomb::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qDebug() <<"ca paint "<<x<<" "<<y<<" "<<size;
    qDebug() <<"bomb painter"<<painter;
    qDebug() <<pixmaps.at(currentPix);
    qDebug() <<pixmaps.at(currentPix)->isQBitmap();
    painter->drawPixmap(x,y,size,size,*(pixmaps.at(currentPix)));
   // pix.paint(painter,option,widget);
}

QRectF QBomb::boundingRect() const
{
    //qDebug() <<"boundingRect"<<pixmaps.at(currentPix)->rect().x()<<" "<<pixmaps.at(currentPix)->rect().y()<<" "<<pixmaps.at(currentPix)->rect().height();
   // return pixmaps.at(currentPix)->rect();
    qDebug()<<"boundingRect"<<bRect.x()<<" "<<bRect.y()<<" "<<bRect.height();
    return bRect;
}
