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

#include <QDataStream>
#include <QDebug>

#include "MapServer.h"
#include "PlayerServer.h"


const QPoint MapServer::dirLeft = QPoint(-1,0);
const QPoint MapServer::dirRight = QPoint(1,0);
const QPoint MapServer::dirUp = QPoint(0,-1);
const QPoint MapServer::dirDown = QPoint(0,1);


MapServer::MapServer()
{
	connect(&timerHeartBeat, SIGNAL(timeout()), this, SLOT(newHeartBeat()));

    int index = 0;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_BOMB;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_FLAME;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_DISEASE;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_OIL;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_KICK;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_FASTER;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_REMOTE;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_MULTIBOMB;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_THROW_GLOVE;
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_BOXING_GLOVE;

    Q_ASSERT(index < BONUS_TABLE_LENGTH);
    while(index < BONUS_TABLE_LENGTH)
        bonusTable[index++] = Bonus::BONUS_NONE;
    debugMode = false;
}

MapServer::~MapServer() {
    foreach(Bonus* b, bonus)
        delete b;
}


void MapServer::loadRandom()
{
	// qDebug() << "set Dimensions (2) "<<width<<" "<<height<<" ";
	qsrand(time(NULL));
	for(int w = 1; w < width-1; w++)
	{
		for(int h = 1; h < height-1; h++)
		{
			//randomly add wall or bricks or nothing
			double d = (double)qrand() / RAND_MAX;
			if(d < 0.05)
				getBlockList()[h*width+w].setType(BlockMapProperty::wall);
			else if(d < 0.1)
				getBlockList()[h*width+w].setType(BlockMapProperty::brick);
			else
				getBlockList()[h*width+w].setType(BlockMapProperty::empty);
		}
	}
	qDebug() << "add walls and bricks";
	//add walls on the map sides
	for(int w = 0; w < width; w++)
	{
		getBlockList()[w].setType(BlockMapProperty::wall);
		getBlockList()[(height-1)*width+w].setType(BlockMapProperty::wall);
	}
	for(int h = 1; h < height-1; h++)
	{
		getBlockList()[h*width].setType(BlockMapProperty::wall);
		getBlockList()[h*width+width-1].setType(BlockMapProperty::wall);
	}
	qDebug() << "add players";

    //randomly add players
    for (int i = 0; i < MAX_NB_PLAYER; i++)
    {
        int x, y;
        int w = (qrand() % (width - 2)) + 1;
        int h = (qrand() % (height - 2)) + 1;
        qDebug() << "Player" << i << ", pos " << w << h;
        w = w * getBlockSize() + getBlockSize() / 2;
        h = h * getBlockSize() + getBlockSize() / 2;
        addPlayerSlot(w,h);
        getBlockPosition(w, h, x, y);
        setType(BlockMapProperty::empty, x, y);
    }
}

/*
int MapServer::getFreePlayerSlot()
{
    int ret = -1;
    QList<initialPlayerPosition>::iterator it = startPlayerSlots.begin();
    for(int cpt = 0; it != startPlayerSlots.end(); ++it, ++cpt)
    {
        if(it->freeSlot)
        {
            ret = cpt;
            it->freeSlot = false;
            setPlayerPosition(ret, it->coord.x(), it->coord.y());
            break;
        }
    }
    return ret;
}
*/

bool MapServer::assignPlayer(int id)
{
    if(id >= startPlayerSlots.size() || !startPlayerSlots.at(id).freeSlot )
        return false;
    newPlayer(id); //update Map Subclass
    startPlayerSlots[id].freeSlot = false;
    setPlayerPosition(id, startPlayerSlots.at(id).coord.x(), startPlayerSlots.at(id).coord.y());
    players[id]->setIsAlive(true);
    return true;
}

void MapServer::addPlayerSlot(int x, int y)
{
    initialPlayerPosition tmp = {QPoint(x,y), true};
    startPlayerSlots.append(tmp);
    maxNbPlayers++;
}

void MapServer::requestOptKey(int id)
{
    players[id]->setOptKey(true);
}

void MapServer::requestBombPlayer(int id) {
	//qDebug() << "Player #" << id << " going " << direction;
	players[id]->raiseLayingBomb();
}

/**
 *   1  2  3
 *    \ | /
 *  0 <- -> 4
 *    / | \
 *   7  6  5
 */
void MapServer::requestMovePlayer(int id, int direction) {
	//qDebug() << "Player #" << id << " going " << direction;
	players[id]->setDirection(direction);
}

bool MapServer::movePlayer(int id, int direction, int distance)
{
	bool ret = false;
	if(direction == 7 || direction == 0 || direction == 1)
		ret = tryMovePlayer(id,MOVE_LEFT,distance);
	if(!ret && (direction == 7 || direction == 6 || direction == 5))
		ret = tryMovePlayer(id,MOVE_UP,distance);
	if(!ret && (direction == 5 || direction == 4 || direction == 3))
		ret = tryMovePlayer(id,MOVE_RIGHT,distance);
	if(!ret && (direction == 1 || direction == 2 || direction == 3))
		ret = tryMovePlayer(id,MOVE_DOWN,distance);
	return ret;
}



/**
 *      1
 *      |
 *  0 <- -> 2
 *      |
 *      3
 */
bool MapServer::tryMovePlayer(int id, int direction, int distance)
{
	/**
	 * Rules for a player move :
	 * Get the block where the player is and :
	 * - 1 if the player is at or beyond the middle of the block (wrt the direction of the move),
	 * and if the next block is empty, we move toward the next block
	 * - 2 if the next block is not empty, we try to circle the block if it is possible
	 * - 3 else the move is rejected
	 */
	qint16 x_player,y_player;
	int move_x = 0;
	int move_y = 0;
	getPlayerPosition( id, x_player, y_player );
	players[id]->setHeading(direction);
	//qDebug() << "Map move player" << id << direction << "x,y" << x << y;
	switch(direction)
	{
	case MOVE_LEFT:
		move_x = -1 * distance;
		break;
	case MOVE_DOWN:
		move_y = -1 * distance;
		break;
	case MOVE_RIGHT:
		move_x = 1 * distance;
		break;
	case MOVE_UP:
		move_y = 1 * distance;
		break;
	default:
		return false;
	}
	int x_originalBlock, y_originalBlock;
	getBlockPosition( x_player, y_player, x_originalBlock, y_originalBlock );
	x_player += move_x + (move_x/distance)*(getBlockSize()/2);
	y_player += move_y + (move_y/distance)*(getBlockSize()/2);
	int x_nextBlock, y_nextBlock;
	getBlockPosition( x_player, y_player, x_nextBlock, y_nextBlock );
	//qDebug() << "next block" << x_nextBlock << y_nextBlock ;
	BlockMapProperty::BlockType typeOfNextBlock = getType(x_nextBlock,y_nextBlock);

	//here we test if the next block is empty and if the next block does not contains a bomb or if the next block is the same as the actual block (if we are before the middle of the block)
	if( (typeOfNextBlock == BlockMapProperty::empty || typeOfNextBlock == BlockMapProperty::flame) &&
        (   (x_originalBlock == x_nextBlock && y_originalBlock == y_nextBlock) ||
            blockContainsBomb(x_nextBlock,y_nextBlock) == -1
        )
      )
	{
		qint16 x,y;
		getPlayerPosition(id,x,y);
		setPlayerPosition(id,x+move_x,y+move_y);
		//We want to stay on the middle of blocks.
		adjustPlayerPosition(id,move_x,move_y, distance);
		return true;
	}
	else
	{
		int pos;
		qint16 x,y;
		getPlayerPosition(id,x,y);
		//can we move closer of the next block ?
		//todo : adjust player position in the case ?
		if(move_x != 0)
		{
			pos = coordinatePositionInBlock(x);
			//qDebug() << "Move closer ?" << pos << move_x;
			if(pos != 0 && ((pos<0 && move_x>0) || (pos>0&&move_x<0)))
			{
				setPlayerPosition(id,x-pos,y);
				return true;
			}
		}
		else
		{
			pos = coordinatePositionInBlock(y);
			if(pos != 0 && ((pos<0 && move_y>0) || (pos>0&&move_y<0)))
			{
				setPlayerPosition(id,x,y-pos);
				return true;
			}
		}

		getPlayerPosition(id,x,y);
		//try to circle the block
		//is this can still happening ? : (player is * and he can go in . )
		//   * #
		//   # .
		if(move_x != 0)
		{
			pos = coordinatePositionInBlock(y);
			int sign = pos > 0 ? 1 : -1;
			if( pos != 0)
			{
				getBlockPosition( x_player, y_player+sign*getBlockSize()/2, x_nextBlock, y_nextBlock );
				typeOfNextBlock = getType(x_nextBlock,y_nextBlock);
				if( (typeOfNextBlock == BlockMapProperty::empty || typeOfNextBlock == BlockMapProperty::flame) &&
                     blockContainsBomb(x_nextBlock,y_nextBlock) == -1)
				{
					setPlayerPosition(id,x+ move_x/2,y+absMin(pos,distance));
					return true;
				}
			}
		}
		else
		{
			pos = coordinatePositionInBlock(x);
			int sign = pos > 0 ? 1 : -1;
			if( pos != 0)
			{
				getBlockPosition( x_player+sign*getBlockSize()/2, y_player, x_nextBlock, y_nextBlock );
				typeOfNextBlock = getType(x_nextBlock,y_nextBlock);
				if( (typeOfNextBlock == BlockMapProperty::empty || typeOfNextBlock == BlockMapProperty::flame) &&
                    blockContainsBomb(x_nextBlock,y_nextBlock) == -1)
				{
					setPlayerPosition(id,x+absMin(pos,distance),y+ move_y/2);
					return true;
				}
			}
		}
	}
	return false;
}

int MapServer::absMin(int a, int b) const
{
	if( b < 0 )
		b = -b;
	if(a > 0)
	{
		return a < b ? a : b;
	}
	else
	{
		a = -a;
		return a < b ? -a : -b;
	}
}

bool MapServer::blockEmpty(int x, int y)
{
    if(getType(x,y) != BlockMapProperty::empty)
        return false;
    if(blockContainsBomb(x,y) != -1)
        return false;
    for (int i=0;i<getNbPlayers();i++)
    {
        qint16 p_x, p_y;
        getPlayerPosition(i, p_x, p_y);
        int p_squareX, p_squareY;
        getBlockPosition(p_x, p_y, p_squareX, p_squareY);
        if (x == p_squareX && y == p_squareY)
            return false;
    }
    QMap<Point<qint8>, Bonus*>::iterator itb = bonus.find(Point<qint8>(x, y));
    if(itb != bonus.end())
        return false;
    return true;
}

bool MapServer::getRandomEmptyPosition(int &x, int &y)
{
    //create a list of empty block
    QList<QPoint> empty;
    for(int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            if(blockEmpty(i,j))
            {
                empty << QPoint(i,j);
            }
        }
    }
    //pick one
    if(empty.empty())
        return false;
    int rand = static_cast<int>((static_cast<double>(qrand()) / RAND_MAX) * empty.size());
    x = empty[rand].x();
    y = empty[rand].y();
    return true;
}

void MapServer::adjustPlayerPosition(int plId, int xDirection, int yDirection, int distance)
{
	qint16 x,y;
	getPlayerPosition(plId,x,y);
	if(xDirection != 0)
	{
		int pos = coordinatePositionInBlock(y);
		setPlayerPosition(plId,x,y-absMin(pos,distance));
	}
	else
	{
		int pos = coordinatePositionInBlock(x);
		setPlayerPosition(plId,x-absMin(pos,distance),y);
	}
}

int MapServer::coordinatePositionInBlock(int coord)
{
	int block = coord / getBlockSize();
	int middle = getBlockSize() * block + getBlockSize()/2;
	if(coord == middle)
		return 0;
	return coord - middle;
	//if(coord < middle)
		//    return -1;
	//return 1;
}

QList<Bomb*> MapServer::addBombMultiple(int playerId)
{
    int squareX,squareY;
    qint16 x,y;
    QList<Bomb*> newBombs;
    getPlayerPosition(playerId,x,y);
    getBlockPosition(x,y,squareX,squareY);
    //drop multibombs if the player already has a bomb here
    if(players[playerId]->getHeading() == -1 || blockContainsBomb(squareX,squareY) != playerId)
        return newBombs;
    while(players[playerId]->getIsBombAvailable())
    {
        getNextBlock(squareX,squareY,squareX,squareY,players[playerId]->getHeading());
        //we check that a player is not here
        bool isPlayer = false;
        for (int i=0;i<getNbPlayers();i++)
        {
            qint16 p_x, p_y;
            getPlayerPosition(i, p_x, p_y);
            int p_squareX, p_squareY;
            getBlockPosition(p_x, p_y, p_squareX, p_squareY);
            if (squareX == p_squareX && squareY == p_squareY)
            {
                qDebug() << "multibomb blocked by player" << i;
                isPlayer = true;
                break;
            }
        }
        //check if a bonus is here
        QMap<Point<qint8>, Bonus*>::iterator itb = bonus.find(Point<qint8>(squareX, squareY));
        if(itb != bonus.end())
        {
            qDebug() << "multibomb blocked by bonus";
            break;
        }

        if(isPlayer)
            break;
        Bomb *b = addBomb(playerId, squareX, squareY);
        if( !b )
            break;
        newBombs << b;
    }
    return newBombs;
}

Bomb* MapServer::addBomb(int playerId)
{
	int squareX,squareY;
	qint16 x,y;
	getPlayerPosition(playerId,x,y);
	getBlockPosition(x,y,squareX,squareY);
	return addBomb(playerId,squareX,squareY);
}

Bomb* MapServer::addBomb(int playerId, int squareX, int squareY)
{
    BlockMapProperty::BlockType type = getType(squareX,squareY);
    if( blockContainsBomb(squareX,squareY) != -1 || type != BlockMapProperty::empty)
        return NULL;

	// add the bomb
	Bomb *newBomb = new Bomb(playerId, squareX, squareY, DEFAULT_BOMB_DURATION, players[playerId]->getFlameLength(), players[playerId]->getRemoteBonus());
	bombs.append(newBomb);
	qDebug() << " MapServer> AddBomb : " << bombs.size() << " BOMBS !!! x: "<<squareX<<" y: "<<squareY<<" bombId: "<<newBomb->bombId;
	players[playerId]->decBombsAvailable();
	return newBomb;
}


Flame* MapServer::explosion(Bomb* b)
{
	bombs.removeOne(b);
	Flame *f = new Flame(b->playerId,20);
	f->addDetonatedBomb(*b);

	QPoint tempPoint = QPoint(b->x,b->y);
	propagateFlame(*f, tempPoint, b->range);

    players[b->playerId]->incBombsAvailable();
	delete b;
	addFlame(f);
	//flames.append(f);

	qDebug()<<"BOOM !";
	return f;
}

void MapServer::propagateFlame(Flame & f, const QPoint & p, int range)
{
	if (!f.getFlamePositions().contains(p))
	{
		f.addFlame(p.x(),p.y());
		for (int i=0;i<getNbPlayers();i++)
		{
			qint16 x,y;
			getPlayerPosition(i,x,y);
			int squareX, squareY;
			getBlockPosition(x,y,squareX,squareY);
			if (p.x()==squareX && p.y()==squareY)
				qDebug() << "player "<<f.getPlayerId()<<" pwned player "<<i;
		}
	}

	directedFlameProgagation(f,p,dirUp,range);
	directedFlameProgagation(f,p,dirDown,range);
	directedFlameProgagation(f,p,dirLeft,range);
	directedFlameProgagation(f,p,dirRight,range);
}

void MapServer::directedFlameProgagation(Flame & f, const QPoint & p, const QPoint & direction, int range){
	QPoint pTemp=p;
	for (int i=0;i<range;i++)
	{
		pTemp=pTemp+direction;
		if ( pTemp.x() < 0 || pTemp.y() < 0 || pTemp.x() >= width || pTemp.y() >= height)
		    return;
		if (getType(pTemp.x(),pTemp.y())==BlockMapProperty::wall ||
			getType(pTemp.x(),pTemp.y())==BlockMapProperty::broken )
			return;

		if (getType(pTemp.x(),pTemp.y())==BlockMapProperty::brick)
		{
			setType(BlockMapProperty::broken, pTemp.x(), pTemp.y());
			f.addBrokenBlock(pTemp.x(),pTemp.y());
			return;
		}

		foreach(Bomb * b, bombs)
		{
			if (b->x == pTemp.x() && b->y == pTemp.y())
			{
				bombs.removeOne(b);
				f.addDetonatedBomb(*b);
				QPoint newPos = QPoint(b->x, b->y);
				propagateFlame(f, newPos, b->range);
                players[b->playerId]->incBombsAvailable();
				delete b;
			}
		}

        // if a bonus is on the square it is destroyed immediately but the flame does not enter
        Bonus* destroyedBonus = removeBonus(pTemp.x(), pTemp.y());
        if(destroyedBonus) {
            delete destroyedBonus;
            return;
        }

        // just add the new square to the list, it's Flame's role to manage that there are no double entries
        f.addFlame(pTemp.x(),pTemp.y());
	}
}

void MapServer::doPlayerDeath(PlayerServer* playerN)
{
    //check if the player has detonators bomb on the field
    foreach(Bomb * b, bombs)
    {
        if (b->playerId == playerN->getId())
        {
            qDebug("dead player has a detonator bomb");
            b->remoteControlled = false;
            b->duration = DEFAULT_BOMB_DURATION;
        }
    }
}

bool MapServer::checkPlayerInFlames(PlayerServer* playerN,
                         const QPoint& playerBlock,
                         const QList<Flame*>& flamesToCheck,
                         QList<killedPlayer>& killedPlayers) {
    foreach(Flame* f, flamesToCheck)
        if(f->getFlamePositions().contains(playerBlock)) {
            playerN->setIsAlive(false);
            killedPlayers.append(killedPlayer(playerN->getId(), f->getPlayerId()));
            return true;
        }

    return false;
}

void MapServer::checkPlayerSurroundings(PlayerServer* playerN,
                                        QList<killedPlayer>& killedPlayers) {
    int x, y;
    getBlockPosition(playerN->getX(), playerN->getY(), x, y);
    QPoint actPoint(x, y);

    // check whether the player threw himself in a flame
    if(checkPlayerInFlames(playerN, actPoint, flames, killedPlayers))
        return;

    // TODO check for other player close by for disease

    Bonus* pickedUpBonus = removeBonus(actPoint.x(), actPoint.y());
    if(pickedUpBonus) {
        // TODO does the code belong to MapServer or to PlayerServer?
        switch(pickedUpBonus->getType()) {
            case Bonus::BONUS_BOMB:
                playerN->incMaxNumberOfBombs();
                break;
            case Bonus::BONUS_FLAME:
                playerN->incFlameLength();
                break;
            case Bonus::BONUS_OIL:
                playerN->setOilBonus(true);
                if(playerN->hasRemoteBonus())
                {
                    playerN->setRemoteBonus(false);
                    bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_REMOTE;
                }
                break;
            case Bonus::BONUS_DISEASE:
            {
                int random = ( qrand() % (SICK_LAST-1) ) + 1;
                playerN->setSickness((sickness)random);
                break;
            }
            case Bonus::BONUS_KICK:
                playerN->setKickBonus();
                break;
            case Bonus::BONUS_FASTER:
                playerN->setFasterBonus();
                break;
            case Bonus::BONUS_REMOTE:
                playerN->setRemoteBonus(true);
                if(playerN->getBoxingGloveBonus())
                {
                    playerN->setBoxingGloveBonus(false);
                    bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_BOXING_GLOVE;
                }
                if(playerN->getOilBonus())
                {
                    playerN->setOilBonus(false);
                    bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_OIL;
                }
                break;
            case Bonus::BONUS_MULTIBOMB:
                playerN->setMultibombBonus(true);
                if(playerN->getThrowbombBonus())
                {
                    playerN->setThrowbombBonus(false);
                    bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_THROW_GLOVE;
                }
                break;
            case Bonus::BONUS_THROW_GLOVE:
                playerN->setThrowbombBonus(true);
                if(playerN->getMultibombBonus())
                {
                    playerN->setMultibombBonus(false);
                    bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_MULTIBOMB;
                }
                break;
            case Bonus::BONUS_BOXING_GLOVE:
                if(playerN->hasRemoteBonus())
                {
                    playerN->setRemoteBonus(false);
                    bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_REMOTE;
                }
                playerN->setBoxingGloveBonus(true);
                break;

            default:
                qDebug() << "Type " << pickedUpBonus->getType() << " not yet implemented!";
        }
        playerN->heldBonus.append(pickedUpBonus);        
    }
}

Bonus* MapServer::removeBonus(qint8 x, qint8 y) {
    QMap<Point<qint8>, Bonus*>::iterator itb = bonus.find(Point<qint8>(x, y));
    if(itb == bonus.end())
        return NULL;

    Bonus* ret = itb.value();

    // TODO check that the bonus cannot be created and destroyed in the same turn
    removedBonus.append(Point<qint8>(ret->getX(), ret->getY()));

    bonus.erase(itb);

    return ret;
}

void MapServer::brokenBlockRemoved(int x, int y) {
    int randomDraw = static_cast<int>((static_cast<double>(qrand()) / RAND_MAX) * BONUS_TABLE_LENGTH);

    if(debugMode)
        randomDraw = randomDraw % 159;

    Bonus::Bonus_t result = bonusTable[randomDraw];
    if(result != Bonus::BONUS_NONE) {
        Bonus* newBonus = new Bonus(result, x, y);
        bonus[Point<qint8>(x,y)] = newBonus;
        createdBonus.append(newBonus);
    }
}

void MapServer::startHeartBeat(qint32 startValue, int intervals) {
	setHeartBeat(startValue);
	timerHeartBeat.start(intervals);
}

void MapServer::newHeartBeat() {
	heartBeat++;
    if(heartBeat % 100 == 0)
        qDebug() << "send Hearbeat #" << heartBeat;

    QList<killedPlayer> killedPlayers;
    createdBonus.clear();
    removedBonus.clear();

	QByteArray updateArray;
	QDataStream updateOut(&updateArray,QIODevice::WriteOnly | QIODevice::Truncate);
	updateOut << heartBeat;

	// start by cleaning flames
	QList<qint16> cleanList;
	QList<Flame*>::iterator itFlame = flames.begin();
	while(itFlame != flames.end()) {
		(*itFlame)->decreaseLifeSpan();
		if( (*itFlame)->isFinished() ) {
			cleanList.append((*itFlame)->getFlameId());
			//TODO, maybe inneficient ? but easier as it correctly deletes broken blocks
			removeFlame((*itFlame)->getFlameId());
            ++itFlame;
			//flames.erase(itFlame++);
		}
		else
		    ++itFlame;
	}
	updateOut << cleanList;

	// now let each alive player lay a bomb, then move
    QList<Bomb*> newBombs;
    if(heartBeat >= 0) {
        foreach(PlayerServer* playerN, players) {
            if(playerN->getIsAlive()) {
                if(playerN->getLayingBomb() || playerN->getSickness() == SICK_DIARRHEA) {
                    if(playerN->getIsBombAvailable())
                    {
                        Bomb* newBomb = addBomb(playerN->getId());
                        if(newBomb != NULL)
                            newBombs << newBomb;
                        else if(playerN->getLayingBomb() && playerN->getMultibombBonus())
                            newBombs << addBombMultiple(playerN->getId());
                    }
                    playerN->clearLayingBomb();
                }
                if(playerN->getDirection() != -1) {
                    movePlayer(playerN->getId(), playerN->getDirection(), playerN->getMoveDistance());
                    playerN->setDirection(-1);
                }
                checkPlayerSurroundings(playerN, killedPlayers);
            }
        }
    }

	// serialize all players information
	updateOut << static_cast<qint8>(players.size());
	foreach(PlayerServer* playerN, players) {
		updateOut << *playerN;
		//update Player infos
		playerN->decreaseDuration();
	}

	// serialize the new bombs
	updateOut << static_cast<qint8>(newBombs.size());
	foreach(Bomb* bombN, newBombs) {
		updateOut << *bombN;
	}

	// then decrease each bomb's counter
	foreach(Bomb* bombN, bombs)
		bombN->decreaseLifeSpan();


	// now we check which bombs must explode
	// WARNING : because a bomb exploding can trigger several other ones
	//           we must restart from the beginning of the list every time
	//           to ensure the iterator is still valid
	QList<Flame*> explodeList;
	QList<Bomb*>::iterator itBomb = bombs.begin();
	while(itBomb != bombs.end()) {
		if((*itBomb)->mustExplode() || ( (*itBomb)->remoteControlled && players[(*itBomb)->playerId]->getOptKey() ) ) {
			explodeList.append(explosion(*itBomb));
			itBomb = bombs.begin();
		}
		else
			++itBomb;
	}

	// serialize the list of explosions
	updateOut << static_cast<qint8>(explodeList.size());
	foreach(Flame* flameN, explodeList)
		updateOut << *flameN;

    // check which players just were killed by the explosions
    if(heartBeat >= 0) {
        foreach(PlayerServer* playerN, players) {
            if(playerN->getIsAlive()) {
                int px, py;
                getBlockPosition(playerN->getX(), playerN->getY(), px, py);
                QPoint actPoint(px, py);
                if(checkPlayerInFlames(playerN, actPoint, explodeList, killedPlayers))
                {
                    doPlayerDeath(playerN);
                }
            }
        }
    }

    // create bonuses if needed
    while(!bonusToSpawn.empty())
    {
        Bonus::Bonus_t bonus_type = bonusToSpawn.takeFirst();
        int x,y;
        qDebug() << "bonus to spawn" << bonus_type;
        if(getRandomEmptyPosition(x,y))
        {
            Bonus* newBonus = new Bonus(bonus_type, x, y);
            bonus[Point<qint8>(x,y)] = newBonus;
            createdBonus.append(newBonus);
        }
        else
        {
            qDebug() << "plus de place";
            bonusToSpawn.clear();
            break;
        }
    }

    // send the list of players that were killed during this heartbeat
    updateOut << killedPlayers;

    // send the list of bonus created during this heartbeat and still available
    updateOut << static_cast<quint8>(createdBonus.size());
    foreach(const Bonus* b, createdBonus)
        updateOut << *b;

    // send the list of bonus removed during this heartbeat
    updateOut << static_cast<quint8>(removedBonus.size());
    foreach(Point<qint8> p, removedBonus) {
        updateOut << p.x();
        updateOut << p.y();
    }

	// send the update to the clients
	emit updatedMap(updateArray);

    // count how many players are still alive
    QList<qint8> playersAlive;
    foreach(PlayerServer* playerN, players) {
        playerN->setOptKey(false);
        if(playerN->getIsAlive()) {
            playersAlive.append(playerN->getId());
            if(playersAlive.size() > 1)
                break;
        }
    }

    // is there a draw? in this case we send -1 as winner's id
    if(playersAlive.size() == 0) {
        timerHeartBeat.stop();
        emit sigWinner(-1);
        return;
    }

    if((! debugMode) && playersAlive.size() == 1) {
        timerHeartBeat.stop();
        emit sigWinner(playersAlive.first());
        return;
    }

    // TODO time over
}
