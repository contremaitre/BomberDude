#ifndef QTB_MAPCLIENT_H
#define QTB_MAPCLIENT_H


#include "Map.h"
#include "Player.h"
#include "Flame.h"

class MapClient : public Map<Player>
{
    Q_OBJECT
public :
    void updateMap(QByteArray& updateBlock);

signals:
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
    void sigHeartbeatUpdated(qint32 value);

    void sigMovePlayer(int player, int x, int y);
    void sigKillPlayer(int);
};


#endif // QTB_MAPCLIENT_H
