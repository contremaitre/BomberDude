#include "MapClient.h"


bool operator<(const QPoint& p1, const QPoint& p2) {
    if(p1.x() == p2.x())
        return p1.y() < p2.y();
    return p1.x() < p2.x();
}


QGraphicsItem* MapClient::addBonus(qint16 x, qint16 y, QGraphicsItem* item) {
    QPoint key(x, y);
    QGraphicsItem* oldItem = 0;

    QMap<QPoint, QGraphicsItem*>::iterator itb = bonus.find(key);
    if(itb != bonus.end())
        oldItem = itb.value();

    bonus[key] = item;

    return oldItem;
}

QGraphicsItem* MapClient::removeBonus(qint16 x, qint16 y) {
    QPoint key(x, y);
    QGraphicsItem* oldItem = 0;

    QMap<QPoint, QGraphicsItem*>::iterator itb = bonus.find(key);
    if(itb != bonus.end()) {
        oldItem = itb.value();
        bonus.erase(itb);
    }

    return oldItem;
}
