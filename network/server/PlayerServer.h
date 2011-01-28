#ifndef PLAYERSERVER_H
#define PLAYERSERVER_H


#include "Player.h"
#include "Bonus.h"

enum sickness { SICK_NONE=0,
                SICK_FAST, //not implemented (no effect)
                //SICK_SLOW,
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

    qint8 getMaxNumberOfBombs() const   { return maxNumberOfBombs; }
    void incMaxNumberOfBombs()          { maxNumberOfBombs++; bombsAvailable++; }

    bool getIsBombAvailable() const;
    void decBombsAvailable()            { bombsAvailable--; }
    void incBombsAvailable()            { bombsAvailable++; }

    qint8 getFlameLength() const        { return flameLength; }
    void incFlameLength()               { flameLength++; }

    void setOilBonus()                  { oilBonus = true; }

    void setSickness(sickness s);
    sickness getSickness()              {return currentSickness;}

    qint16 getMoveDistance() const;

    void decreaseDuration(); //only used for sickness for the moment
    QList<Bonus*> heldBonus;

private:
    bool layingBomb;
    bool oilBonus;

    qint8 maxNumberOfBombs;
    qint8 bombsAvailable;

    qint8 flameLength;
    sickness currentSickness;
    qint16 SicknessDuration;

    quint16 moveDistance;
};


#endif // PLAYERSERVER_H
