#ifndef QTB_MAPCLIENT_H
#define QTB_MAPCLIENT_H


#include "Map.h"
#include "Player.h"


class MapClient : public Map<Player>
{
    Q_OBJECT

signals:
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
    void sigHeartbeatUpdated(qint32 value);
};


#endif // QTB_MAPCLIENT_H
