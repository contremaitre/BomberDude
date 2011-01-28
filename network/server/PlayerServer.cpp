#include "PlayerServer.h"
#include "constant.h"

#include <QDebug>

PlayerServer::PlayerServer(int playerId) :
    Player(playerId),
    layingBomb(false),
    oilBonus(false),
    maxNumberOfBombs(DEFAULT_BOMB_CAPACITY),
    bombsAvailable(maxNumberOfBombs),
    flameLength(DEFAULT_BOMB_RANGE),
    currentSickness(SICK_NONE),
    SicknessDuration(0)

{}

void PlayerServer::setSickness(sickness s)
{
    currentSickness = s;
    Q_ASSERT(s >= SICK_NONE && s < SICK_LAST );
    qDebug() << "sickness" << s;
    if(currentSickness == SICK_NONE)
        isSick = false;
    else
    {
        isSick = true;
        SicknessDuration = DEFAULT_DISEASE_DURATION;
    }
}

void PlayerServer::decreaseDuration()
{
    //qDebug() << "decrease" << isSick << SicknessDuration;
    if(isSick)
    {
        SicknessDuration--;
        if(SicknessDuration <= 0)
            setSickness(SICK_NONE);
    }
}

PlayerServer::~PlayerServer() {
    // TODO release the bonus on the map instead of deleting them
    foreach(Bonus* b, heldBonus)
        delete b;
}
