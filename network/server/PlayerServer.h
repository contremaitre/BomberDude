#ifndef PLAYERSERVER_H
#define PLAYERSERVER_H


#include "Player.h"
#include "Bonus.h"

enum sickness { SICK_NONE=0,
                SICK_FAST,
                SICK_SLOW,
                SICK_NO_BOMB,
                SICK_DIARRHEA,
                SICK_LAST,
            };

class PlayerServer: public Player
{
public:
    PlayerServer(int playerId);
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

    bool getIsBombAvailable() const;
    void decBombsAvailable()            { bombsAvailable--; }
    void incBombsAvailable()            { bombsAvailable++; }

    qint8 getFlameLength() const        { return flameLength; }
    void incFlameLength()               { flameLength++; }

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

    void setFasterBonus()               { moveDistance++; }

    void setRemoteBonus(bool);
    bool hasRemoteBonus() const         { return remoteBonus; }
    bool getRemoteBonus();

    bool getOnTeleport() const          { return onTeleport; }
    void setOnTeleport(bool on)         { onTeleport = on; }

    void setSickness(sickness s);
    sickness getSickness()              {return currentSickness;}

    qint16 getMoveDistance() const;

    void decreaseDuration(); //only used for sickness for the moment
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
