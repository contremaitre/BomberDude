#include "QGraphicsCaseItem.h"
#include <QPainter>

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

