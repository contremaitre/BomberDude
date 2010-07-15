#include "QGraphicsCaseItem.h"

QGraphicsCaseItem::QGraphicsCaseItem()
{
    rectItem = new QGraphicsRectItem();
}

QGraphicsCaseItem::QGraphicsCaseItem(int x_a, int y_a, int x_b, int y_b)
{
    rectItem = new QGraphicsRectItem(QRectF(x_a, y_a, x_b, y_b));
}

void QGraphicsCaseItem::setPos(int x_a, int y_a, int x_b, int y_b)
{
    rectItem->setRect(QRectF(x_a, y_a, x_b, y_b));
}
