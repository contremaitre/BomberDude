#ifndef BOMBSERVER_H
#define BOMBSERVER_H


#include "Bomb.h"
#include "Map.h"


class BombServer: public Bomb<PixelToBlock>
{
    Q_OBJECT

public:
	BombServer(qint8 playerId,
               qint16 x,
               qint16 y,
               int duration,
               int range,
               bool remote,
               bool oil,
               qint16 blockSize);
    virtual ~BombServer();

	void decreaseLifeSpan()					{ if(!getIsRC()) duration--; }

	bool mustExplode() const                { return !getIsRC() && duration < 0; }

    void setDuration(int duration)          { this->duration = duration; }

    int getRange() const                    { return range; }

    bool getHasOil() const                  { return hasOil; }

    globalDirection getDirection() const    { return direction; }
    void setDirection(globalDirection dir)  { direction = dir; }

signals:
    void sigTileChanged(qint16 bombId, qint8 oldx, qint8 oldy, qint8 newx, qint8 newy);

private:
	static qint16 index;

    int duration;                           /// duration in heartbeats
    int range;
    bool hasOil;
    globalDirection direction;
};


#endif // BOMBSERVER_H
