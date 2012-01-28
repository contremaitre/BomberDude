/*
    Copyright (C) 2011 Thomas Kister

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
               PixelToBlock ptb);
    virtual ~BombServer();

	void decreaseLifeSpan()					{ if(!getIsRC()) duration--; }

	bool mustExplode() const                { return !getIsRC() && duration < 0; }

    void setDuration(int duration)          { this->duration = duration; }

    int getRange() const                    { return range; }

    bool getHasOil() const                  { return hasOil; }

    void setDudBomb()                       { dud_bomb = true; }

    globalDirection getDirection() const    { return direction; }
    void setDirection(globalDirection dir)  { direction = dir; }

    bool getHasMoved() const                { return hasMoved; }
    void setHasMoved(bool m)                { hasMoved = m; }

signals:
    void sigTileChanged(qint16 bombId, qint8 oldx, qint8 oldy, qint8 newx, qint8 newy);
    void sigFlyingBombChange(qint16 bombId);

private:
	static qint16 index;

    int duration;                           /// duration in heartbeats
    int range;
    bool hasOil;
    globalDirection direction;
    bool hasMoved;                          /// true when the bomb started to move

};


#endif // BOMBSERVER_H
