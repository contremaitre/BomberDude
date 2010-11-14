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

#ifndef QTB_QGRAPHICSSQUAREITEM_H
#define QTB_QGRAPHICSSQUAREITEM_H


#include <QGraphicsPixmapItem>
#include <QGraphicsObject>


enum caseType
{
    VOID,
    BOMBERMAN,
    BOMB,
    
};

class QGraphicsSquareItem : public QGraphicsObject
{
private:
    int x,y,size;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
    QRectF boundingRect() const;
    QGraphicsPixmapItem pix;
public:
    QGraphicsSquareItem();
    QGraphicsSquareItem(int,int,int);
    void setPos(int, int, int);
    void setItem(caseType);
    void setItem(const QPixmap &);
    QGraphicsPixmapItem *getItem();
};


#endif // QTB_QGRAPHICSSQUAREITEM_H
