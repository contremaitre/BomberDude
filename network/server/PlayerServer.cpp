#include "PlayerServer.h"

PlayerServer::PlayerServer(int playerId) :
    Player(playerId),
    layingBomb(false),
    maxNumberOfBombs(3),
    bombsAvailable(maxNumberOfBombs),
    flameLength(4)
{}
