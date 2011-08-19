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

#ifndef QANIMATEDITEM_H
#define QANIMATEDITEM_H

#include <QGraphicsItem>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsObject>

class QAnimatedItem : public QGraphicsItem
{
private:
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
        QRectF boundingRect() const;
        virtual void initPixList()=0;

public:
        int size, currentPix;
        QList<QPixmap*>* currentAnim;
        QAnimatedItem();
        void setPos(int,int,int);
        void nextFrame();
};

#endif // QANIMATEDITEM_H
