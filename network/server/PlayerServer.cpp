#include "PlayerServer.h"
#include "constant.h"

PlayerServer::PlayerServer(int playerId) :
    Player(playerId),
    layingBomb(false),
    maxNumberOfBombs(DEFAULT_BOMB_CAPACITY),
    bombsAvailable(maxNumberOfBombs),
    flameLength(DEFAULT_BOMB_RANGE)
{}
