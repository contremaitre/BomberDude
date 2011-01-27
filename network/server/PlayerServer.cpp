#include "PlayerServer.h"
#include "constant.h"


PlayerServer::PlayerServer(int playerId) :
    Player(playerId),
    layingBomb(false),
    oilBonus(false),
    maxNumberOfBombs(DEFAULT_BOMB_CAPACITY),
    bombsAvailable(maxNumberOfBombs),
    flameLength(DEFAULT_BOMB_RANGE),
    currentSickness(SICK_NONE)

{}

void PlayerServer::setSickness(sickness s)
{
    currentSickness = s;
    if(currentSickness == SICK_NONE)
        isSick = false;
    else
        isSick = true;
}

PlayerServer::~PlayerServer() {
    // TODO release the bonus on the map instead of deleting them
    foreach(Bonus* b, heldBonus)
        delete b;
}
