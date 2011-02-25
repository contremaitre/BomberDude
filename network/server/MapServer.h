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

#ifndef QTB_MAPSERVER_H
#define QTB_MAPSERVER_H


#include "Map.h"
#include "PlayerServer.h"
#include "Bonus.h"
#include "Point.h"


enum optionDirection {optDirNone,optDirLeft,optDirRight,optDirUp,optDirDown};

struct optionCoord
{
    enum optionDirection direction;
    int x;
    int y;
};

struct mapStyle
{
    QString name;
    BlockMapProperty::BlockOption option;
    QList<optionCoord> coordList;
};

class MapServer : public Map<PlayerServer,mapStyle>
{
	Q_OBJECT

private:
    virtual void brokenBlockRemoved(int x, int y);
	Flame* explosion(Bomb* b);
	void propagateFlame(Flame & f, const QPoint & p, int range);
	void directedFlameProgagation(Flame & f, const QPoint & p, const QPoint & direction, int range);
    void adjustPlayerPosition(int plId, int xDirection, int yDirection, int distance);
    //Test if a coordinate is bellow (-1) on (0) or above (1) the middle of the block
    int coordinatePositionInBlock(int coord);
    int absMin(int a, int b) const;
    bool movePlayer(int id, int direction, int distance);
    bool tryMovePlayer(int id, int direction, int distance);
    int keyToDirection(int direction);
    Bomb* addBomb(int id);
    Bomb* addBomb(int id, int x, int y);
    QList<Bomb*> addBombMultiple(int playerId);
    bool getRandomEmptyPosition(int &x, int &y);
    bool blockEmpty(int x, int y);//return true if there is nothing on this block
    bool blockContainsTeleport(int x, int y, int &id);
    void getNextTeleportPosition(int id, int &x, int &y);
    bool checkPlayerInFlames(PlayerServer* playerN,
                             const QPoint& playerBlock,
                             const QList<Flame*>& flamesToCheck);
    void doPlayerDeath(PlayerServer* playerN); //check what has to be done when a player die
    void checkPlayerSurroundings(PlayerServer* player);

    Bonus* removeBonus(qint8 x, qint8 y);

	QTimer timerHeartBeat;
    bool debugMode;

    QMap<Point<qint8>, Bonus*> bonus;                       ///< list of bonus
    static const int BONUS_TABLE_LENGTH = 352;
    Bonus::Bonus_t bonusTable[BONUS_TABLE_LENGTH];          ///< table to determine which bonus to add
    QList<const Bonus*> createdBonus;                       ///< list of bonus created during the heartbeat
    QList<Point<qint8> > removedBonus;                      ///< list of bonus picked up or destroyed during the heartbeat
    QList<QPoint> teleports;                                ///< We need a list of teleports to know where is the next teleport

	struct initialPlayerPosition
	{
	    QPoint coord;
	    bool freeSlot;
	};
	QList <initialPlayerPosition> startPlayerSlots;
	QList <Bonus::Bonus_t> bonusToSpawn; //when a player die, or pick a bonus incompatible with another one, the bonus re-spawn on the map
	QList<killedPlayer> killedPlayers; //list of killed players during the current heartbeat
	static const QPoint dirLeft;
	static const QPoint dirRight;
	static const QPoint dirUp;
	static const QPoint dirDown;

public:
	MapServer();
//    MapServer(qint16, qint16, qint16);
    virtual ~MapServer();

    virtual void addStyle(const mapStyle &style);
    void selectStyle(int);
	void loadRandom();
	void addPlayerSlot(int, int);
	bool assignPlayer(int id);
	//int getFreePlayerSlot();
	void requestMovePlayer(int id, int direction);
	void requestOptKey(int id);
	void requestBombPlayer(int id);

    void setDebugMode(bool val)                                 { debugMode = val; }
	void startHeartBeat(qint32 startValue, int intervals);

signals:
	void updatedMap(QByteArray data);

	/* Unused */
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
	void sigHeartbeatUpdated(qint32 value);
	/* End unused */

    void sigWinner(qint8 playerId);

public slots:
	void newHeartBeat();

};

//We only need this operator in the server, and it must match the >> operator in MapClient
QDataStream& operator<<(QDataStream& out, const mapStyle& ms);

#endif // QTB_MAPSERVER_H
