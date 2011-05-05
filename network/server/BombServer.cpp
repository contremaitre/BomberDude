
#include "BombServer.h"


qint16 BombServer::index=1;


BombServer::BombServer(qint8 playerId,
                       qint16 x,
                       qint16 y,
                       int duration,
                       int range,
                       bool remote,
                       bool oil,
                       PixelToBlock ptb) :
    Bomb<PixelToBlock>(index++, playerId, x, y, remote, ptb),
    duration(duration),
    range(range),
    hasOil(oil),
    direction(dirNone),
    hasMoved(false)
{}

BombServer::~BombServer()
{}
