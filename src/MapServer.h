/*
    Copyright (C) 2010 Sébastien Escudier

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

#ifndef MAPSERVER_H
#define MAPSERVER_H


#include "Map.h"

class MapServer : public Map
{
	Q_OBJECT

private:
	void propagateFlame(Flame & f, const QPoint & p, int range);
	void directedFlameProgagation(Flame & f, const QPoint & p, const QPoint & direction, int range);
    void adjustPlayerPosition(int plId, int xDirection, int yDirection);
    //Test if a coordinate is bellow (-1) on (0) or above (1) the middle of the block
    int coordinatePositionInBlock(int coord);
    int absMin(int a, int b) const;
	bool movePlayer(int id, int direction);
	bool movePlayerLeft(int id);
    bool movePlayerRight(int id);
    bool movePlayerUp(int id);
    bool movePlayerDown(int id);
    bool movePlayerOld(int id, int direction);
    Bomb* addBomb(int id);
    Bomb* addBomb(int id, int x, int y);

	QTimer timerHeartBeat;

	struct initialPlayerPosition
	{
	    QPoint coord;
	    bool freeSlot;
	};
	QList <initialPlayerPosition> startPlayerSlots;

	static const QPoint dirLeft;
	static const QPoint dirRight;
	static const QPoint dirUp;
	static const QPoint dirDown;

public:
	MapServer();
//    MapServer(qint16, qint16, qint16);
//    ~MapServer();
	void loadRandom();
	void addPlayerSlot(int, int);
	int getFreePlayerSlot();
	void requestMovePlayer(int id, int direction);
	void requestBombPlayer(int id);

	void startHeartBeat(qint32 startValue, int intervals);

private:
	const Flame* explosion(Bomb* b);

signals:
	void updatedMap(QByteArray data);

public slots:
	void newHeartBeat();

};


#endif
