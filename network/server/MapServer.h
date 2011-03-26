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


#include <QTimer>

#include "Map.h"
#include "PlayerServer.h"
#include "BombServer.h"
#include "Bonus.h"
#include "Point.h"


struct optionCoord
{
    globalDirection direction;
    int x;
    int y;
};

struct mapStyle
{
    QString name;
    BlockMapProperty::BlockOption option;
    QList<optionCoord> coordList;
};

class MapServer : public Map<PlayerServer,BombServer,mapStyle>
{
	Q_OBJECT

private:
    virtual void brokenBlockRemoved(int x, int y);
	Flame* explosion(BombServer* b);
	void propagateFlame(Flame & f, const QPoint & p, int range);
	void directedFlameProgagation(Flame & f, const QPoint & p, const QPoint & direction, int range);
    void adjustPlayerPosition(int plId, int xDirection, int yDirection, int distance);
    //Test if a coordinate is bellow (-1) on (0) or above (1) the middle of the block
    int coordinatePositionInBlock(int coord);
    int absMin(int a, int b) const;
    bool movePlayer(int id, int direction, int distance);
    bool tryMovePlayer(int id, globalDirection direction, int distance);

    /**
      Tries to move the bomb on the map.

      Depending on the value of direction, we have two cases:
      - dirNone: the bomb moves because it was kicked, get the real direction information from the bomb itself
      - others:  the bomb moves on a walkway, some bonus like oil must be ignored

      @param b          Pointer to the bomb
      @param direction  Which way to move it
      @return           true if the bomb moved, false otherwise
      */
    bool tryMoveBomb(BombServer* b, globalDirection direction);

    int keyToDirection(int direction);
    BombServer* addBomb(int id);
    BombServer* addBomb(int id, int x, int y);
    QList<BombServer*> addBombMultiple(int playerId);
    bool getRandomEmptyPosition(qint16 &x, qint16 &y);
    bool blockEmpty(int x, int y);//return true if there is nothing on this block
    bool blockContainsPlayer(int x, int y);
    bool blockContainsTeleport(int x, int y, int &id);
    void getNextTeleportPosition(int id, int &x, int &y);
    bool checkPlayerInFlames(PlayerServer* playerN,
                             const QPoint& playerBlock,
                             const QList<Flame*>& flamesToCheck);
    void doPlayerDeath(PlayerServer* playerN, int killedBy); //check what has to be done when a player die
    void checkPlayerSurroundings(PlayerServer* player);
    void exchangePlayersPositions();
    void exchangePlayersPositions(PlayerServer *p1, PlayerServer *p2);
    bool shrinkMap();
    Bonus* removeBonus(qint8 x, qint8 y);

    static globalDirection reverseDirection(globalDirection initialDir);

	QTimer timerHeartBeat;
    bool debugMode;
    bool spawningBlocks;

    QMap<Point<qint8>, Bonus*> bonus;                       ///< list of bonus
    static const int BONUS_TABLE_LENGTH = NB_BONUS*16*2;    ///< todo
    Bonus::Bonus_t bonusTable[BONUS_TABLE_LENGTH];          ///< table to determine which bonus to add
    QList<const Bonus*> createdBonus;                       ///< list of bonus created during the heartbeat
    QList<Point<qint8> > removedBonus;                      ///< list of bonus picked up or destroyed during the heartbeat
    QList<QPoint> teleports;                                ///< We need a list of teleports to know where is the next teleport

    QPoint shrink; //current block to add when playtime expired
    QPoint shrinkLimitUp;
    QPoint shrinkLimitDown;
    QPoint shrinkDirection;

	struct initialPlayerPosition
	{
	    QPoint coord;
	    bool freeSlot;
	};
	QList <initialPlayerPosition> startPlayerSlots;
	QList <Bonus::Bonus_t> bonusToSpawn; //when a player die, or pick a bonus incompatible with another one, the bonus re-spawn on the map
	QList<killedPlayer> killedPlayers; //list of killed players during the current heartbeat
	static const QPoint planeDirLeft;
	static const QPoint planeDirRight;
	static const QPoint planeDirUp;
	static const QPoint planeDirDown;

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
