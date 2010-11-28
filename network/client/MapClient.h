#ifndef QTB_MAPCLIENT_H
#define QTB_MAPCLIENT_H


#include "Map.h"
#include "Player.h"
#include "Flame.h"
#include "Bonus.h"


// necessary to use QPoint in a QMap
bool operator<(const QPoint&, const QPoint&);


class QGraphicsItem;

class MapClient : public Map<Player>
{
    Q_OBJECT
public :
    void updateMap(QByteArray& updateBlock);

private:
    QMap<QPoint, QGraphicsItem*> bonus;

signals:
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
    void sigHeartbeatUpdated(qint32 value);

    void sigMovePlayer(int player, int x, int y);
    void sigKillPlayer(int);
    void sigAddBonus(Bonus::Bonus_t type, qint16 x, qint16 y);
    void sigRemoveBonus(qint16 x, qint16 y);
};


#endif // QTB_MAPCLIENT_H
