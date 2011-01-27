#ifndef PLAYERSERVER_H
#define PLAYERSERVER_H


#include "Player.h"
#include "Bonus.h"

enum sickness { SICK_NONE,
                SICK_UNKNOWN,
                SICK_FAST,
                SICK_NO_BOMB,
                SICK_DIARRHEA
            };

class PlayerServer: public Player
{
public:
    PlayerServer(int playerId);
    ~PlayerServer();

    // these methods modify the variables inherited from Player
    void setIsAlive(bool isAlive)       { this->isAlive = isAlive; }
    void setSickness(sickness s);

    // methods specific to PlayerServer
	void raiseLayingBomb()				{ layingBomb = true; }
	void clearLayingBomb()				{ layingBomb = false; }
	bool getLayingBomb() const			{ return layingBomb; }

    qint8 getMaxNumberOfBombs() const   { return maxNumberOfBombs; }
    void incMaxNumberOfBombs()          { maxNumberOfBombs++; bombsAvailable++; }

    bool getIsBombAvailable() const     { return bombsAvailable != 0; }
    void decBombsAvailable()            { bombsAvailable--; }
    void incBombsAvailable()            { bombsAvailable++; }

    qint8 getFlameLength() const        { return flameLength; }
    void incFlameLength()               { flameLength++; }

    void setOilBonus()                  { oilBonus = true; }

    QList<Bonus*> heldBonus;

private:
    bool layingBomb;
    bool oilBonus;

    qint8 maxNumberOfBombs;
    qint8 bombsAvailable;

    qint8 flameLength;
    sickness currentSickness;

};


#endif // PLAYERSERVER_H
