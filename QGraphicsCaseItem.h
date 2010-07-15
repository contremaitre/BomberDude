#ifndef QGRAPHICSCASEITEM_H
#define QGRAPHICSCASEITEM_H
#include <QGraphicsRectItem>

class QGraphicsCaseItem : public QGraphicsRectItem
{
private:
    QGraphicsPixmapItem pixItem;
public:
    QGraphicsCaseItem();
    QGraphicsCaseItem(int, int, int, int);
    void setPos(int, int, int, int);
    QGraphicsRectItem *rectItem;
};

#endif

