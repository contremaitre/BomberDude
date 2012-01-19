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
#include "QAnimateditem.h"

QAnimatedItem::QAnimatedItem()
{
    size=0;
    currentPix=0;
}


void QAnimatedItem::setPos(int x, int y, int size)
{
    setX(x);
    setY(y);
    this->size=size;
}

QRectF QAnimatedItem::boundingRect() const
{
    return QRectF(0,0,size,size);
}


void QAnimatedItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_ASSERT(!currentAnim->empty() && currentPix < currentAnim->size() );
    painter->drawPixmap(0,0,size,size,*(currentAnim->at(currentPix)));
}



void QAnimatedItem::nextFrame()
{
    if (currentPix>=currentAnim->size()-1)
        currentPix=0;
    else
        currentPix++;
    update();
}

void QAnimatedItem::appendNewFrame(QList<QPixmap*>* anim, const QString &path)
{
    QPixmap * tmpPix= new QPixmap(path);
    anim->append(tmpPix);
}
