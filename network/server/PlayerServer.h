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

#ifndef PLAYERSERVER_H
#define PLAYERSERVER_H


#include "Player.h"
#include "Bonus.h"

class PlayerServer: public Player
{
public:

    typedef int sickness;
    static const sickness SICK_NONE = 0;
    static const sickness SICK_FAST = 0x1;
    static const sickness SICK_SLOW = 0x2;
    static const sickness SICK_NO_BOMB = 0x4;
    static const sickness SICK_DIARRHEA = 0x8;
    static const sickness SICK_SMALL_FLAME = 0x10;
    static const sickness SICK_SHUFFLE = 0x20;
    static const sickness SICK_LAST = 7; //total number

    PlayerServer(qint8 playerId);
    ~PlayerServer();

    // these methods modify the variables inherited from Player
    void setIsAlive(bool isAlive)       { this->isAlive = isAlive; }

    // methods specific to PlayerServer
	void raiseLayingBomb()				{ layingBomb = true; }
	void clearLayingBomb()				{ layingBomb = false; }
	bool getLayingBomb() const			{ return layingBomb; }

    void setOptKey(bool o)              { optKeyClicked = o; }
    bool getOptKey();

    qint8 getMaxNumberOfBombs() const   { return maxNumberOfBombs; }
    void incMaxNumberOfBombs();

    qint8 getFlameLength() const;
    void incFlameLength();
    qint8 getNbFlameBonus() const;

    bool getIsBombAvailable() const;
    void decBombsAvailable()            { bombsAvailable--; }
    void incBombsAvailable()            { bombsAvailable++; }

    void setOilBonus(bool set)          { oilBonus = set; }
    bool getOilBonus() const            { return oilBonus; }

    void setKickBonus()                 { kickBonus = true; }
    bool getKickBonus() const           { return kickBonus; }

    void setMultibombBonus(bool set)    { multiBombBonus = set; }
    bool getMultibombBonus() const      { return multiBombBonus; }

    void setThrowbombBonus(bool set)    { throwBombBonus = set; }
    bool getThrowbombBonus() const      { return throwBombBonus; }

    void setBoxingGloveBonus(bool set)  { boxingGloveBonus = set; }
    bool getBoxingGloveBonus() const    { return boxingGloveBonus; }

    void setRemoteBonus(bool);
    bool hasRemoteBonus() const         { return remoteBonus; }
    bool getRemoteBonus();

    void setGoldFlame()                 { goldFlame = true; }
    bool getGoldFlame() const           { return goldFlame; }

    bool getOnTeleport() const          { return onTeleport; }
    void setOnTeleport(bool on)         { onTeleport = on; }

    void setSickness(sickness s);
    sickness getSickness() const        { return currentSickness; }

    qint16 getMoveDistance() const;
    void setFasterBonus();

    void decreaseDuration(); //only used for sickness for the moment
    qint16 getSicknessDuration() const  { return SicknessDuration; }
    QList<Bonus*> heldBonus;

private:
    bool layingBomb;
    bool oilBonus;
    bool kickBonus;
    bool multiBombBonus;
    bool throwBombBonus;
    bool boxingGloveBonus;
    bool remoteBonus;
    bool optKeyClicked;
    bool goldFlame;

    bool onTeleport; //set if the player was on a teleport on his previous position

    qint8 maxNumberOfBombs;
    qint8 bombsAvailable;
    qint8 nbRemoteControl;

    qint8 flameLength;
    sickness currentSickness;
    qint16 SicknessDuration;

    quint16 moveDistance;
};


#endif // PLAYERSERVER_H
