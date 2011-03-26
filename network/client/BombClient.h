#ifndef BOMBCLIENT_H
#define BOMBCLIENT_H


#include "Bomb.h"
#include "Map.h"


class BombClient : public Bomb<PixelToBlock>
{
    Q_OBJECT

public:
    BombClient(PixelToBlock ptc);
    virtual ~BombClient();

signals:
    void sigTileChanged(qint16 bombId, qint8 oldx, qint8 oldy, qint8 newx, qint8 newy);
};


#endif // BOMBCLIENT_H
