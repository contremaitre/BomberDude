#include "PlayerServer.h"
#include "constant.h"


PlayerServer::PlayerServer(int playerId) :
    Player(playerId),
    layingBomb(false),
    maxNumberOfBombs(DEFAULT_BOMB_CAPACITY),
    bombsAvailable(maxNumberOfBombs),
    flameLength(DEFAULT_BOMB_RANGE)
{}

PlayerServer::~PlayerServer() {
    // TODO release the bonus on the map instead of deleting them
    foreach(Bonus* b, heldBonus)
        delete b;
}
