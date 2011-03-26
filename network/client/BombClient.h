#ifndef BOMBCLIENT_H
#define BOMBCLIENT_H


#include "Bomb.h"


class BombClient : public Bomb<int>
{
    Q_OBJECT

public:
    BombClient();
    virtual ~BombClient();

signals:
    void sigTileChanged();
};


#endif // BOMBCLIENT_H
