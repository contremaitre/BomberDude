#ifndef PLAYERSERVER_H
#define PLAYERSERVER_H


#include "Player.h"
#include "Bonus.h"


class PlayerServer: public Player
{
public:
    PlayerServer(int playerId);
    ~PlayerServer();

    // these methods modify the variables inherited from Player
    void setIsDead()                    { isAlive = false; }
    void setSickness(sickness s)        { currentSickness = s; }

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

    QList<Bonus*> heldBonus;

private:
	bool layingBomb;

    qint8 maxNumberOfBombs;
    qint8 bombsAvailable;

    qint8 flameLength;

};


#endif // PLAYERSERVER_H
