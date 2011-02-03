#include "PlayerServer.h"
#include "constant.h"

#include <QDebug>

PlayerServer::PlayerServer(int playerId) :
    Player(playerId),
    layingBomb(false),
    oilBonus(false),
    kickBonus(false),
    maxNumberOfBombs(DEFAULT_BOMB_CAPACITY),
    bombsAvailable(maxNumberOfBombs),
    flameLength(DEFAULT_BOMB_RANGE),
    currentSickness(SICK_NONE),
    SicknessDuration(0),
    moveDistance(MOVE_STEP)

{}

void PlayerServer::setSickness(sickness s)
{
    currentSickness = s;
    Q_ASSERT(s >= SICK_NONE && s < SICK_LAST );
    qDebug() << "sickness :";
    switch(s)
    {
    case SICK_DIARRHEA:
        qDebug() << "diarrhea";
        break;
    case SICK_FAST:
        qDebug() << "fast";
        break;
    case SICK_SLOW:
        qDebug() << "slow";
        break;
    case SICK_NO_BOMB:
        qDebug() << "no bomb";
        break;
    case SICK_NONE:
        qDebug() << "none";
        break;
    default:
        qDebug() << "unknown disease ??";
        break;
    }
    if(currentSickness == SICK_NONE)
        isSick = false;
    else
    {
        isSick = true;
        SicknessDuration = DEFAULT_DISEASE_DURATION;
    }
}

bool PlayerServer::getIsBombAvailable() const
{
    if(currentSickness == SICK_NO_BOMB)
        return false;
    return bombsAvailable != 0;
}

qint16 PlayerServer::getMoveDistance() const
{
    if(currentSickness == SICK_SLOW)
        return moveDistance / 2;
    else if(currentSickness == SICK_FAST)
        return moveDistance * 3;
    return moveDistance;
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
