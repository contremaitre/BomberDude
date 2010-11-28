#ifndef QTB_MAPCLIENT_H
#define QTB_MAPCLIENT_H


#include "Map.h"
#include "Player.h"
#include "Bonus.h"


// necessary to use QPoint in a QMap
bool operator<(const QPoint&, const QPoint&);


class QGraphicsItem;

class MapClient : public Map<Player>
{
    Q_OBJECT

public:
    QGraphicsItem* addBonus(qint16 x, qint16 y, QGraphicsItem*);
    QGraphicsItem* removeBonus(qint16 x, qint16 y);

private:
    QMap<QPoint, QGraphicsItem*> bonus;

signals:
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
    void sigHeartbeatUpdated(qint32 value);
};


#endif // QTB_MAPCLIENT_H
