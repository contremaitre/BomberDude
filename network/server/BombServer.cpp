
#include "BombServer.h"


qint16 BombServer::index=1;


BombServer::BombServer(qint8 playerId,
                       qint16 x,
                       qint16 y,
                       int duration,
                       int range,
                       bool remote,
                       bool oil) :
    Bomb(index++, playerId, remote),
    duration(duration),
    range(range),
    hasOil(oil),
    direction(dirNone)
{
    setX(x);
    setY(y);
}
