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

#include <QtGlobal>
#include <QDataStream>
#include <QDebug>

#include "MapServer.h"


const QPoint MapServer::dirLeft = QPoint(-1,0);
const QPoint MapServer::dirRight = QPoint(1,0);
const QPoint MapServer::dirUp = QPoint(0,-1);
const QPoint MapServer::dirDown = QPoint(0,1);



MapServer::MapServer()
{
	connect(&timerHeartBeat, SIGNAL(timeout()), this, SLOT(newHeartBeat()));
}

void MapServer::loadRandom()
{
	// qDebug() << "set Dimensions (2) "<<width<<" "<<height<<" ";
	qsrand(time(NULL));
	for(int w = 1; w < getWidth()-1; w++)
	{
		for(int h = 1; h < getHeight()-1; h++)
		{
			//randomly add wall or bricks or nothing
			double d = (double)qrand() / RAND_MAX;
			if(d < 0.05)
				getBlockList()[h*getWidth()+w].setType(BlockMapProperty::wall);
			else if(d < 0.1)
				getBlockList()[h*getWidth()+w].setType(BlockMapProperty::brick);
			else
				getBlockList()[h*getWidth()+w].setType(BlockMapProperty::empty);
		}
	}
	qDebug() << "add walls and bricks";
	//add walls on the map sides
	for(int w = 0; w < getWidth(); w++)
	{
		getBlockList()[w].setType(BlockMapProperty::wall);
		getBlockList()[(getHeight()-1)*getWidth()+w].setType(BlockMapProperty::wall);
	}
	for(int h = 1; h < getHeight()-1; h++)
	{
		getBlockList()[h*getWidth()].setType(BlockMapProperty::wall);
		getBlockList()[h*getWidth()+getWidth()-1].setType(BlockMapProperty::wall);
	}
	qDebug() << "add players";

    //randomly add players
    for (int i = 0; i < getMaxNbPlayers(); i++)
    {
        int x, y;
        int w = (qrand() % (getWidth() - 2)) + 1;
        int h = (qrand() % (getHeight() - 2)) + 1;
        qDebug() << "Player" << i << ", pos " << w << h;
        w = w * getBlockSize() + getBlockSize() / 2;
        h = h * getBlockSize() + getBlockSize() / 2;
        setPlayerPosition(i, w, h);
        getBlockPosition(w, h, x, y);
        setType(BlockMapProperty::empty, x, y);
    }
}

void MapServer::requestBombPlayer(int id) {
	QList<Player*>::iterator itPlayer = players.begin();
	while(itPlayer != players.end())
		if((*itPlayer)->getId() == id)
			break;
	if(itPlayer == players.end()) {
		qDebug() << "Unknown player #" << id;
		return;
	}
	//qDebug() << "Player #" << id << " going " << direction;
	(*itPlayer)->raiseLayingBomb();
}

/**
 *   1  2  3
 *    \ | /
 *  0 <- -> 4
 *    / | \
 *   7  6  5
 */
void MapServer::requestMovePlayer(int id, int direction) {
	QList<Player*>::iterator itPlayer = players.begin();
	while(itPlayer != players.end())
		if((*itPlayer)->getId() == id)
			break;
	if(itPlayer == players.end()) {
		qDebug() << "Unknown player #" << id;
		return;
	}
	//qDebug() << "Player #" << id << " going " << direction;
	(*itPlayer)->setDirection(direction);
}

bool MapServer::movePlayer(int id, int direction)
{
	bool ret = false;
	if(direction == 7 || direction == 0 || direction == 1)
		ret = movePlayerLeft(id);
	if(!ret && (direction == 7 || direction == 6 || direction == 5))
		ret = movePlayerDown(id);
	if(!ret && (direction == 5 || direction == 4 || direction == 3))
		ret = movePlayerRight(id);
	if(!ret && (direction == 1 || direction == 2 || direction == 3))
		ret = movePlayerUp(id);
	return ret;
}
//TODO optimize this
bool MapServer::movePlayerLeft(int id)
{
	return movePlayerOld(id,0);
}

bool MapServer::movePlayerDown(int id)
{
	return movePlayerOld(id,3);
}

bool MapServer::movePlayerRight(int id)
{
	return movePlayerOld(id,2);
}

bool MapServer::movePlayerUp(int id)
{
	return movePlayerOld(id,1);
}


/**
 *      1
 *      |
 *  0 <- -> 2
 *      |
 *      3
 */
bool MapServer::movePlayerOld(int id, int direction)
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
	//qDebug() << "Map move player" << id << direction << "x,y" << x << y;
	switch(direction)
	{
	case 0:
		move_x = -1 * MOVE_STEP;
		break;
	case 1:
		move_y = -1 * MOVE_STEP;
		break;
	case 2:
		move_x = 1 * MOVE_STEP;
		break;
	case 3:
		move_y = 1 * MOVE_STEP;
		break;
	default:
		return false;
	}
	int x_originalBlock, y_originalBlock;
	getBlockPosition( x_player, y_player, x_originalBlock, y_originalBlock );
	x_player += move_x + (move_x/(MOVE_STEP))*(getBlockSize()/2);
	y_player += move_y + (move_y/(MOVE_STEP))*(getBlockSize()/2);
	int x_nextBlock, y_nextBlock;
	getBlockPosition( x_player, y_player, x_nextBlock, y_nextBlock );
	//qDebug() << "next block" << x_nextBlock << y_nextBlock ;
	BlockMapProperty::BlockType typeOfNextBlock = getType(x_nextBlock,y_nextBlock);

	//here we test if the next block is empty and if the next block does not contains a bomb or if the next block is the same as the actual block (if we are before the middle of the block)
	if( typeOfNextBlock == BlockMapProperty::empty && (((x_originalBlock==x_nextBlock) && (y_originalBlock==y_nextBlock))||!blockContainsBomb(x_nextBlock,y_nextBlock)))
	{
		qint16 x,y;
		getPlayerPosition(id,x,y);
		setPlayerPosition(id,x+move_x,y+move_y);
		//We want to stay on the middle of blocks.
		adjustPlayerPosition(id,move_x,move_y);
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
				if( typeOfNextBlock == BlockMapProperty::empty && !blockContainsBomb(x_nextBlock,y_nextBlock))
				{
					setPlayerPosition(id,x+ move_x/2,y+absMin(pos,MOVE_STEP));
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
				if( typeOfNextBlock == BlockMapProperty::empty && !blockContainsBomb(x_nextBlock,y_nextBlock))
				{
					setPlayerPosition(id,x+absMin(pos,MOVE_STEP),y+ move_y/2);
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

void MapServer::adjustPlayerPosition(int plId, int xDirection, int yDirection)
{
	qint16 x,y;
	getPlayerPosition(plId,x,y);
	if(xDirection != 0)
	{
		int pos = coordinatePositionInBlock(y);
		setPlayerPosition(plId,x,y-absMin(pos,MOVE_STEP));
	}
	else
	{
		int pos = coordinatePositionInBlock(x);
		setPlayerPosition(plId,x-absMin(pos,MOVE_STEP),y);
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

//to be used by server only
Bomb* MapServer::bomb(int playerId)
{
	int squareX,squareY;
	qint16 x,y;
	getPlayerPosition(playerId,x,y);
	getBlockPosition(x,y,squareX,squareY);
	return bomb(playerId,squareX,squareY);
}

Bomb* MapServer::bomb(int playerId, int squareX, int squareY)
{
	// is there a bomb at the same place ?
	foreach (Bomb *b, *getBombList())
	{
		if((b->x == squareX) && (b->y == squareY))
			return 0;
	}

	// add the bomb
	Bomb *newBomb = new Bomb(3, playerId, 150, squareX, squareY);
	getBombList()->append(newBomb);
	qDebug() << " MapServer> AddBomb : " << getBombList()->size() << " BOMBS !!! x: "<<squareX<<" y: "<<squareY<<" bombId: "<<newBomb->bombId;
	return newBomb;
}


const Flame* MapServer::explosion(Bomb* b)
{
	//getBombList()->removeOne(b);
	Flame *f = new Flame(b->playerId,20);
	QPoint tempPoint = QPoint(b->x,b->y);
	propagateFlame(*f, tempPoint, b->range);
	getFlameList()->append(f);

	qDebug()<<"BOOM !";
	return f;
}

void MapServer::propagateFlame(Flame & f, const QPoint & p, int range)
{
	if (!f.getFlamePositions().contains(p))
	{
		f.addFlame(p.x(),p.y());
		for (int i=0;i<MAX_NB_PLAYER;i++)
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
		if (getType(pTemp.x(),pTemp.y())==BlockMapProperty::wall)
			return;

		if (getType(pTemp.x(),pTemp.y())==BlockMapProperty::brick)
		{
			setType(BlockMapProperty::empty, pTemp.x(), pTemp.y());
			f.addBrokenBlock(pTemp.x(),pTemp.y());
			return;
		}

		foreach (Bomb * b,*getBombList())
		{
			if (b->x == pTemp.x() && b->y == pTemp.y())
			{
				getBombList()->removeOne(b);
				QPoint newPos = QPoint(b->x, b->y);
				propagateFlame(f, newPos, b->range);
				delete b;
			}
		}

		if (!f.getFlamePositions().contains(pTemp))
		{
			f.addFlame(pTemp.x(),pTemp.y());
			for (int i=0;i<MAX_NB_PLAYER;i++)
			{
				qint16 x,y;
				getPlayerPosition(i,x,y);
				int squareX, squareY;
				getBlockPosition(x,y,squareX,squareY);
				if (pTemp.x()==squareX && pTemp.y()==squareY)
					qDebug() << "player "<<f.getPlayerId()<<" pwned player "<<i;
			}
		}
	}
}

void MapServer::startHeartBeat(qint32 startValue, int intervals) {
	setHeartBeat(startValue);
	timerHeartBeat.start(intervals);
}

void MapServer::newHeartBeat() {
	heartBeat++;
	//qDebug() << "Hearbeat #" << heartBeat;

	QByteArray updateArray;
	QDataStream updateOut(&updateArray,QIODevice::WriteOnly | QIODevice::Truncate);
	updateOut << heartBeat;

	// start by cleaning flames
	QList<Flame*> cleanList;
	QList<Flame*>::iterator itFlame = flames.begin();
	while(itFlame != flames.end()) {
		(*itFlame)->decreaseLifeSpan();
		if( (*itFlame)->isFinished() ) {
			cleanList.append(*itFlame);
			flames.erase(itFlame++);
		}
		else
			++itFlame;
	}
	//updateOut << cleanList;
	foreach(Flame* flame, cleanList)
		delete flame;
	cleanList.clear();

	// now let each alive player lay a bomb, then move
	QList<Player*> movedPlayers;
	QList<Bomb*> newBombs;
	foreach(Player* playerN, players) {
		if(playerN->getLayingBomb()) {
			playerN->clearLayingBomb();
			Bomb* newBomb = bomb(playerN->getId());
			if(newBomb != 0)
				newBombs.append(newBomb);
		}
		if(playerN->getDirection() != -1) {
			movePlayer(playerN->getId(), playerN->getDirection());
			playerN->setDirection(-1);
			movedPlayers.append(playerN);
		}
	}
	updateOut << (qint8) movedPlayers.size();
	foreach(Player* playerN, movedPlayers) {
		updateOut << *playerN;
	}

	updateOut << players;

	// then make explode some bombs

	// WARNING : it is very important to have a list or a similar container for which iterators remain
	// valid even when other elements are removed, because a bomb can trigger several other ones
	QList<const Flame*> explodeList;
	QList<Bomb*>::iterator itBomb = bombs.begin();
	while(itBomb != bombs.end()) {
		(*itBomb)->decreaseLifeSpan();
		if((*itBomb)->mustExplode()) {
			explodeList.append(explosion(*itBomb));
			delete *itBomb;
			bombs.erase(itBomb++);
		}
		else
			++itBomb;
	}
	//updateOut << explodeList;
	// don't delete the pointers, they're still held in MapServer's "flames"
	explodeList.clear();

	// send the update to the clients
	emit updatedMap(updateArray);
}
