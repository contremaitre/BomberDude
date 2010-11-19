#ifndef QTB_MAPCLIENT_H
#define QTB_MAPCLIENT_H


#include "Map.h"
#include "Player.h"


class MapClient : public QObject, public Map<Player>
{
    Q_OBJECT

private:
    virtual void emitSigBlockChanged(int pos)               { emit sigBlockChanged(pos); }
    virtual void emitSigBlockChanged(int i, int j)          { emit sigBlockChanged(i, j); }
    virtual void emitSigHeartbeatUpdated(qint32 value)      { emit sigHeartbeatUpdated(value); }

signals:
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
    void sigHeartbeatUpdated(qint32 value);
};


#endif // QTB_MAPCLIENT_H
