/*
    Copyright (C) 2010,2011,2012 Sébastien Escudier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PlayerServer.h"
#include "constant.h"

#include <QDebug>

PlayerServer::PlayerServer(qint8 playerId) :
    Player(playerId),
    layingBomb(false),
    oilBonus(false),
    kickBonus(false),
    multiBombBonus(false),
    throwBombBonus(false),
    boxingGloveBonus(false),
    remoteBonus(false),
    optKeyClicked(false),
    goldFlame(false),
    onTeleport(false),
    maxNumberOfBombs(DEFAULT_BOMB_CAPACITY),
    bombsAvailable(maxNumberOfBombs),
    nbRemoteControl(0),
    flameLength(DEFAULT_FLAME_LENGTH),
    currentSickness(SICK_NONE),
    SicknessDuration(0),
    moveDistance(MOVE_STEP)

{}

void PlayerServer::setSickness(sickness s)
{
    int OR =  s | currentSickness;
    currentSickness = static_cast<sickness>(OR);
    Q_ASSERT(s >= SICK_NONE && s < (1 << (SICK_LAST - 2)) );
    qDebug() << "sickness :";
    switch(s)
    {
    case SICK_DIARRHEA:
        qDebug() << "diarrhea";
        break;
    case SICK_FAST:
        qDebug() << "fast";
        if(currentSickness & SICK_SLOW )
            currentSickness &= ~SICK_SLOW;
        break;
    case SICK_SLOW:
        qDebug() << "slow";
        if(currentSickness & SICK_FAST )
            currentSickness &= ~SICK_FAST;
        break;
    case SICK_NO_BOMB:
        qDebug() << "no bomb";
        break;
    case SICK_SMALL_FLAME:
        qDebug() << "small flame";
        break;
    case SICK_NONE:
        qDebug() << "none";
        currentSickness = SICK_NONE;
        break;
    default:
        qDebug() << "unknown or multiple disease";
        break;
    }
    if(currentSickness == SICK_NONE)
        isSick = false;
    else
    {
        isSick = true;
        SicknessDuration = DISEASE_DURATION;
    }
}

bool PlayerServer::getRemoteBonus()
{
    if(nbRemoteControl > 0)
    {
        nbRemoteControl--;
        return true;
    }
    return false;
}

void PlayerServer::setRemoteBonus(bool set)
{
    remoteBonus = set;
    if(set)
        nbRemoteControl = maxNumberOfBombs;
    else
        nbRemoteControl = 0;
}

void PlayerServer::setFasterBonus()
{
    if(moveDistance - (MOVE_STEP) < BONUS_MAX_SKATE)
        moveDistance++;
}

qint8 PlayerServer::getFlameLength() const
{
    if( currentSickness & SICK_SMALL_FLAME)
        return 1;
    if( goldFlame )
        return BONUS_MAX_FLAME_LENGTH;
    return flameLength;
}

qint8 PlayerServer::getNbFlameBonus() const
{
    return flameLength - DEFAULT_FLAME_LENGTH;
}

void PlayerServer::incFlameLength()
{
    if(flameLength - DEFAULT_FLAME_LENGTH < BONUS_MAX_FLAME_LENGTH)
        flameLength++;
}

void PlayerServer::incMaxNumberOfBombs()
{
    if(maxNumberOfBombs >= BONUS_MAX_BOMBS )
        return;
    maxNumberOfBombs++;
    bombsAvailable++;
    if(remoteBonus)
        nbRemoteControl++;
}


bool PlayerServer::getIsBombAvailable() const
{
    if(currentSickness & SICK_NO_BOMB)
        return false;
    return bombsAvailable != 0;
}

qint16 PlayerServer::getMoveDistance() const
{
    if(currentSickness & SICK_SLOW)
        return moveDistance / 2;
    else if(currentSickness & SICK_FAST)
        return moveDistance * 3;
    return moveDistance;
}

bool PlayerServer::getOptKey()
{
    bool ret = optKeyClicked;
    optKeyClicked = false; //only one controlled bomb at a time
    return ret;
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
