#include "Bomb.h"

Bomb::Bomb(int range, int playerId, int duration)
{
    this->range = range;
    this->playerId = playerId;
    this->duration = duration;
    blinkTimer = new QTimer(this);
}

Bomb::~Bomb()
{
}

