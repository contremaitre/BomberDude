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

#include "QGraphicsCaseItem.h"

QGraphicsCaseItem::QGraphicsCaseItem()
{
    x = 0;
    y = 0;
    size = 0;
}

QGraphicsCaseItem::QGraphicsCaseItem(int x, int y, int size)
{
    setPos(x,y,size);
}

void QGraphicsCaseItem::setPos(int x, int y, int size)
{
    this->x = x;
    this->y = y;
    this->size = size;
    setOffset(QPointF(x, y));
}

QGraphicsPixmapItem *QGraphicsCaseItem::getItem()
{
    return this;
}

void QGraphicsCaseItem::setItem(const QPixmap &p)
{
    setPixmap(p);
}

void QGraphicsCaseItem::setItem(caseType t)
{
    switch(t)
    {
        case BOMBERMAN:
            //QPixmap p("pictures/tux.png");
            //QPixmap ps = p.scaled(QSizeF(size,size).toSize());
            //QPainter painter(&p);
            // Scale QGraphicsPixmapItem to wanted 'size' and keep the aspect ratio
            //QSize pixmapSize = p.size();
            //setPixmap(bomberman);
            //scaled(QSizeF(size,size).toSize(), Qt::KeepAspectRatio);
        break;
    }
}

