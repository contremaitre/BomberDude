/*
    Copyright (C) 2010,2011 Sébastien Escudier

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


const QPoint MapServer::planeDirLeft = QPoint(-1,0);
const QPoint MapServer::planeDirRight = QPoint(1,0);
const QPoint MapServer::planeDirUp = QPoint(0,-1);
const QPoint MapServer::planeDirDown = QPoint(0,1);


MapServer::MapServer()
          : debugMode(false),
            spawningBlocks(false),
            shrink(-1,0),
            shrinkDirection(planeDirRight)
{
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
    for(int i = 0; i < 16; i++, index++)
        bonusTable[index] = Bonus::BONUS_RANDOM;

    Q_ASSERT(index < BONUS_TABLE_LENGTH);
    while(index < BONUS_TABLE_LENGTH)
        bonusTable[index++] = Bonus::BONUS_NONE;

    mapStyle randomSpawning;
    randomSpawning.name = "random spawning";
    randomSpawning.option = BlockMapProperty::random_spawn;
    addStyle(randomSpawning);
}

MapServer::~MapServer() {
}

void MapServer::addStyle(const mapStyle &style)
{
    styles << style;
    //qDebug() << "MapServer, style added, size = " << styles.size() << ", positions = " << style.coordList.size();
}

void MapServer::selectStyle(int style)
{
    /* sanity check */
    if(style >= styles.size())
        return;

    BlockMapProperty::BlockOption option = styles[style].option;
    if(option == BlockMapProperty::random_spawn)
    {
        spawningBlocks = true;
        return;
    }
    foreach(optionCoord coord,styles[style].coordList)
    {
        setOption(coord.x, coord.y, option, coord.direction);
        if(option == BlockMapProperty::teleport)
        {
            QPoint point(coord.x, coord.y);
            teleports << point;
            //remove adjacent block
            block_list[coord.y * width + coord.x].setType(BlockMapProperty::empty);
            if(coord.x > 1)
                block_list[coord.y * width + coord.x-1].setType(BlockMapProperty::empty);
            if(coord.x < width-1)
                block_list[coord.y * width + coord.x+1].setType(BlockMapProperty::empty);
            if(coord.y > 1)
                block_list[(coord.y-1) * width + coord.x].setType(BlockMapProperty::empty);
            if(coord.y < height-1)
                block_list[(coord.y+1) * width + coord.x].setType(BlockMapProperty::empty);

        }
    }
}

void MapServer::loadRandom()
{
    // qDebug() << "set Dimensions (2) "<<width<<" "<<height<<" ";
    qsrand( time(NULL));
    for (int w = 1; w < width - 1; w++)
    {
        for (int h = 1; h < height - 1; h++)
        {
            //randomly add wall or bricks or nothing
            double d = (double) qrand() / RAND_MAX;
            if (d < 0.05)
                block_list[h * width + w].setType(BlockMapProperty::wall);
            else if (d < 0.1)
                block_list[h * width + w].setType(BlockMapProperty::brick);
            else
                block_list[h * width + w].setType(BlockMapProperty::empty);
        }
    }
    qDebug() << "add walls and bricks";
    //add walls on the map sides
    for (int w = 0; w < width; w++)
    {
        block_list[w].setType(BlockMapProperty::wall);
        block_list[(height - 1) * width + w].setType(BlockMapProperty::wall);
    }
    for (int h = 1; h < height - 1; h++)
    {
        block_list[h * width].setType(BlockMapProperty::wall);
        block_list[h * width + width - 1].setType(BlockMapProperty::wall);
    }
    qDebug() << "add players";

    //randomly add players
    for (int i = 0; i < MAX_NB_PLAYER; i++)
    {
        QPoint pos;
        int w = (qrand() % (width - 2)) + 1;
        int h = (qrand() % (height - 2)) + 1;
        w = w * getBlockSize() + getBlockSize() / 2;
        h = h * getBlockSize() + getBlockSize() / 2;
        addPlayerSlot(w,h);
        pos = getBlockPosition(w, h);
        setType(BlockMapProperty::empty, pos.x(), pos.y());
    }
}

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

globalDirection MapServer::movePlayer(int id, int direction, int distance, bool &againstWall)
{
    if (direction == 7 || direction == 0 || direction == 1)
        if (tryMovePlayer(id, dirLeft, distance, againstWall))
            return dirLeft;
    if (direction == 7 || direction == 6 || direction == 5)
        if (tryMovePlayer(id, dirUp, distance, againstWall))
            return dirUp;
    if (direction == 5 || direction == 4 || direction == 3)
        if (tryMovePlayer(id, dirRight, distance, againstWall))
            return dirRight;
    if (direction == 1 || direction == 2 || direction == 3)
        if (tryMovePlayer(id, dirDown, distance, againstWall))
            return dirDown;
    return dirNone;
}



/**
 *      1
 *      |
 *  0 <- -> 2
 *      |
 *      3
 */
bool MapServer::tryMovePlayer(int id, globalDirection direction, int distance, bool &againstWall)
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
    const qint16 x_origPixel = x_player;
    const qint16 y_origPixel = y_player;

	players[id]->setHeading(direction);
	//qDebug() << "Map move player" << id << direction << "x,y" << x << y;
	switch(direction)
	{
        case dirLeft:
            move_x = -1 * distance;
            break;
        case dirDown:
            move_y = -1 * distance;
            break;
        case dirRight:
            move_x = 1 * distance;
            break;
        case dirUp:
            move_y = 1 * distance;
            break;
        default:
            Q_ASSERT(false);
            //return false;
	}
	QPoint originalBlock = getBlockPosition( x_player, y_player );
	x_player += move_x + (move_x/distance)*(getBlockSize()/2);
	y_player += move_y + (move_y/distance)*(getBlockSize()/2);
	QPoint nextBlock = getBlockPosition( x_player, y_player );
	//qDebug() << "next block" << nextBlock.x() << nextBlock.y() ;
	BlockMapProperty::BlockType typeOfNextBlock = getType(nextBlock.x(),nextBlock.y());

    // We store the result since we use it at least once
    BombServer* bombOnNextBlock = getTileBomb(nextBlock.x(), nextBlock.y());

    // Can we kick a bomb? First check that the bomb is on another tile
    if(originalBlock.x() != nextBlock.x() || originalBlock.y() != nextBlock.y()) {
        if(bombOnNextBlock && players[id]->getKickBonus()) {
            // check that the player is at the center of the tile so he can kick
            QPoint centerOfTile = getCenterCoordForBlock(originalBlock.x(), originalBlock.y());
            if( ( (direction == dirLeft || direction == dirRight) && y_origPixel == centerOfTile.y() ) ||
                ( (direction == dirUp   || direction == dirDown)  && x_origPixel == centerOfTile.x() )
              )
            {
                bombOnNextBlock->setDirection(direction);
                setPlayerPosition(id, centerOfTile.x(), centerOfTile.y());
                return true;
            }
        }
    }

    //here we test if the next block is empty and if the next block does not contains a bomb or if the next block is the same as the actual block (if we are before the middle of the block)
    if( typeOfNextBlock == BlockMapProperty::empty &&
        (   (originalBlock.x() == nextBlock.x() && originalBlock.y() == nextBlock.y()) ||
            bombOnNextBlock == 0
        )
      )
    {
		setPlayerPosition(id,players[id]->getX()+move_x,players[id]->getY()+move_y);
		//We want to stay on the middle of blocks.
		adjustPlayerPosition(id,move_x,move_y, distance);
		return true;
	}
	else
	{
		int pos;
		//can we move closer of the next block ?
		//todo : adjust player position in the case ?
		if(move_x != 0)
		{
			pos = coordinatePositionInBlock(players[id]->getX());
			//qDebug() << "Move closer ?" << pos << move_x;
			if (pos != 0 && ((pos < 0 && move_x > 0) || (pos > 0 && move_x < 0)))
			{
				setPlayerPosition(id,players[id]->getX()-pos,players[id]->getY());
				againstWall = true;
				return true;
			}
		}
		else
		{
			pos = coordinatePositionInBlock(players[id]->getY());
			if(pos != 0 && ((pos<0 && move_y>0) || (pos>0&&move_y<0)))
			{
				setPlayerPosition(id,players[id]->getX(),players[id]->getY()-pos);
				return true;
			}
		}

		//try to circle the block
		if(move_x != 0)
		{
			pos = coordinatePositionInBlock(players[id]->getY());
			int sign = pos > 0 ? 1 : -1;
			if( pos != 0)
			{
                nextBlock = getBlockPosition( x_player, y_player+sign*getBlockSize()/2 );
                typeOfNextBlock = getType(nextBlock.x(),nextBlock.y());
                if(typeOfNextBlock == BlockMapProperty::empty && getTileBomb(nextBlock.x(),nextBlock.y()) == NULL)
				{
					setPlayerPosition(id,players[id]->getX()+ move_x/2,players[id]->getY()+absMin(pos,distance));
					return true;
				}
			}
		}
		else
		{
			pos = coordinatePositionInBlock(players[id]->getX());
			int sign = pos > 0 ? 1 : -1;
			if( pos != 0)
			{
                nextBlock = getBlockPosition( x_player+sign*getBlockSize()/2, y_player );
                typeOfNextBlock = getType(nextBlock.x(),nextBlock.y());
                if(typeOfNextBlock == BlockMapProperty::empty && getTileBomb(nextBlock.x(),nextBlock.y()) == 0)
				{
					setPlayerPosition(id,players[id]->getX()+absMin(pos,distance),players[id]->getY()+ move_y/2);
					return true;
				}
			}
		}
	}
	return false;
}

void MapServer::throwBomb(BombServer *bomb, globalDirection direction, int distance)
{
    QPoint bombPoint(bomb->getTileX(), bomb->getTileY());
    for(int i = 0 ; i < distance; ++i)
        getNextBlock(bombPoint.x(), bombPoint.y(), bombPoint.rx(), bombPoint.ry(), direction, true);
    if(!bomb->getFlying())
        bomb->setFlying(true);
    bomb->setDirection(direction);
    bomb->setDestination(bombPoint);
    bomb->setFlHeartbeat(heartBeat - 20); //todo remove this value
    //qDebug() << "throwing a bomb" << (int)direction << bombPoint.x() << bombPoint.y();
}

bool MapServer::moveFlyingBomb(BombServer* b)
{
    //qDebug() << "moveFlyingBomb" << heartBeat << b->getFlHeartbeat();
    if(b->getFlHeartbeat() == heartBeat)
    {
        QPoint dest = b->getDestination();
        bool bEmply = getType(dest.x(),dest.y()) == BlockMapProperty::empty
                      && !blockContainsPlayer(dest.x(), dest.y()) && !getTileBomb(dest.x(), dest.y());
        QPoint centre = getCenterCoordForBlock(dest.x(), dest.y());
        b->setX(centre.x());
        b->setY(centre.y());
        //qDebug("flying bomb arrived");
        if(bEmply)
        {
            b->setFlying(false);
            b->setDirection(dirNone);
            //tiles[dest.x()][dest.y()].withBomb = b; todo
        }
        else
        {
            if( b->getHasOil() && (qrand() % OIL_BOMB_RANDOM_DIR) == 0 )
            {
                /* random direction (between 0 et 2 to exclude the previous position */
                int randomDraw = qrand()%3;
                if(b->getDirection() % 2 == randomDraw % 2 && randomDraw != b->getDirection()) //opposite direction
                    randomDraw = 3;
                throwBomb(b, static_cast<globalDirection>(randomDraw), 1);
            }
            else
                throwBomb(b, b->getDirection(), 1);
        }
        return true;
    }
    return false;
}

bool MapServer::tryMoveBomb(BombServer* b, globalDirection direction)
{
    int distance = WALKWAY_SPEED;
    bool isOnWalkway = true;

    QPoint originalBlock = getBlockPosition( b->getX(), b->getY());
    QPoint centreOrig = getCenterCoordForBlock(originalBlock.x(), originalBlock.y());

    if(direction == dirNone) {
        // before any computation, we check whether the bomb is on the centre of the tile
        // in which case we must take into account arrows that change its direction
        if( getOption(originalBlock.x(), originalBlock.y()) == BlockMapProperty::arrow &&
            centreOrig.x() == b->getX() && centreOrig.y() == b->getY() )
        {
            b->setDirection(getOptionDirection(originalBlock.x(), originalBlock.y()));
        }

        direction = b->getDirection();
        distance = MOVE_STEP;
        isOnWalkway = false;
    }

	int newx = b->getX();
	int newy = b->getY();

	int move_x = 0;
	int move_y = 0;
    int offset = distance + (getBlockSize()/2);

	switch(direction)
	{
        case dirLeft:
            move_x = -distance;
            newx -= offset;
            break;
        case dirDown:
            move_y = -distance;
            newy -= offset;
            break;
        case dirRight:
            move_x = distance;
            newx += offset;
            break;
        case dirUp:
            move_y = distance;
            newy += offset;
            break;
        default:
            Q_ASSERT(false);
	}


	QPoint nextBlock = getBlockPosition( newx, newy );
	//qDebug() << "next block" << nextBlock.x() << nextBlock.y() ;
	BlockMapProperty::BlockType typeOfNextBlock = getType(nextBlock.x(),nextBlock.y());

    // recenter the bomb, if the bomb is moving in a given direction it must be centered along the other axis
    if(direction == dirLeft || direction == dirRight) {
        int offset = coordinatePositionInBlock(b->getY());
        if(offset != 0) {
            b->setY(b->getY() - offset);
            return true;
        }
    }
    else {
        int offset = coordinatePositionInBlock(b->getX());
        if(offset != 0) {
            b->setX(b->getX() - offset);
            return true;
        }
    }

    // before any move, we check if the bomb is going to cross the centre of the tile
    // in that case we must take arrows into account
    //
    // s-------d  c         not crossing (yet)
    // s---c---d            crossing
    //
    // s is the starting pixel, d the destination pixel. We must check if the centre is on
    // that segment, however we must not include d itself, it will be dealt with during the
    // next movement. And no need to check s because it's been dealt with earlier in the method.
    //
    // We compute the difference between s and c, c and d. If the signs are different,
    // c is between them. In which case we limit the amount of movement so the bomb finishes
    // its move on the center of the tile, where its direction can finally be changed.

    if(getOption(originalBlock.x(), originalBlock.y()) == BlockMapProperty::arrow) {
        if(b->getX() == centreOrig.x()) {
            int off_sy = centreOrig.y() - b->getY();
            int off_dy = centreOrig.y() - newy;

            if( (off_sy > 0 && off_dy < 0) || (off_sy < 0 && off_dy > 0) )
                newy += off_dy;
        }
        else {
            Q_ASSERT(b->getY() == centreOrig.y());

            int off_sx = centreOrig.x() - b->getX();
            int off_dx = centreOrig.x() - newx;

            if( (off_sx > 0 && off_dx < 0) || (off_sx < 0 && off_dx > 0) )
                newx += off_dx;
        }
    }

    // we need to check the tile on which the bomb will overlap after the move
    // if it is the same than the next tile (i.e. the bomb rolled past the middle of the current tile)
    // then we check whether a player is standing there, in which case the bomb must bounce back.
    QPoint overlapTile = getOverlappingBlockPosition(b->getX(), b->getY());
    int anbx, anby;
    getNextBlock(originalBlock.x(), originalBlock.y(), anbx, anby, direction);
    if( anbx == overlapTile.x() &&
        anby == overlapTile.y() &&
        blockContainsPlayer(anbx, anby) )
    {
        b->setX(centreOrig.x());
        b->setY(centreOrig.y());

        if(b->getHasOil() && !isOnWalkway)
            b->setDirection(reverseDirection(b->getDirection()));
        else
            b->setDirection(dirNone);
        return true;
    }

    // here we test if the next block is empty 
    // and if the next block is the same as the actual block or does not contains a bomb nor a player
    if( typeOfNextBlock == BlockMapProperty::empty &&
        (   (originalBlock.x() == nextBlock.x() && originalBlock.y() == nextBlock.y()) ||
            (   getTileBomb(nextBlock.x(),nextBlock.y()) == 0 &&
                ! blockContainsPlayer(nextBlock.x(), nextBlock.y())
            )
        )
      )
    {
        b->setX(b->getX() + move_x);
        b->setY(b->getY() + move_y);
		return true;
	}
	else
	{
        // TODO we should check first for an arrow
        // we hit something, maybe the oil option was enabled?
        if(b->getHasOil() && !isOnWalkway)
            b->setDirection(reverseDirection(b->getDirection()));

		// can we move closer to the next block ?
		if(move_x != 0)
		{
			int pos = coordinatePositionInBlock(b->getX());
			//qDebug() << "Move closer ?" << pos << move_x;
			if((pos<0 && move_x>0) || (pos>0 && move_x<0))
			{
                b->setX(b->getX() - pos);
				return true;
			}
		}
		else
		{
			int pos = coordinatePositionInBlock(b->getY());
			if((pos<0 && move_y>0) || (pos>0 && move_y<0))
			{
				b->setY(b->getY() - pos);
				return true;
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

void MapServer::getNextTeleportPosition(int id, int &x, int &y)
{
    id++;
    if(id >= teleports.size())
        id = 0;
    x = teleports[id].x();
    y = teleports[id].y();
}

bool MapServer::blockContainsTeleport(int x, int y, int &id)
{
    if(getOption(x,y) != BlockMapProperty::teleport)
        return false;
    for(int i = 0; i < teleports.size(); i++)
    {
        if(teleports[i].x() == x && teleports[i].y() == y)
        {
            id = i;
            return true;
        }
    }
    return false;
}

bool MapServer::blockEmpty(int x, int y)
{
    if(getType(x,y) != BlockMapProperty::empty)
        return false;
    if(getTileBomb(x, y) != 0)
        return false;
    if(blockContainsPlayer(x, y))
        return false;
    if(getTileBonus(x, y) != 0)
        return false;
    return true;
}

bool MapServer::blockContainsPlayer(int x, int y)
{
    for (int i=0;i<getNbPlayers();i++)
    {
        qint16 p_x, p_y;
        getPlayerPosition(i, p_x, p_y);
        QPoint block = getBlockPosition(p_x, p_y);
        if (x == block.x() && y == block.y())
            return true;
    }
    return false;
}

bool MapServer::getRandomEmptyPosition(qint16 &x, qint16 &y)
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
	if(xDirection != 0)
	{
		int pos = coordinatePositionInBlock(players[plId]->getY());
		setPlayerPosition(plId,players[plId]->getX(),players[plId]->getY()-absMin(pos,distance));
	}
	else
	{
		int pos = coordinatePositionInBlock(players[plId]->getX());
		setPlayerPosition(plId,players[plId]->getX()-absMin(pos,distance),players[plId]->getY());
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

QList<BombServer*> MapServer::addBombMultiple(int playerId)
{
    QList<BombServer*> newBombs;
    QPoint block = getBlockPosition(players[playerId]->getX(),players[playerId]->getY());
    //drop multibombs if the player already has a bomb here
    int blockBombPlayerId = -1;
    BombServer* bomb = getTileBomb(block.x(),block.y());
    if(bomb)
        blockBombPlayerId = bomb->getPlayerId();
    if(players[playerId]->getHeading() == -1 || blockBombPlayerId != playerId)
        return newBombs;
    while(players[playerId]->getIsBombAvailable())
    {
        getNextBlock(block.x(),block.y(),block.rx(),block.ry(),players[playerId]->getHeading());
        //we check that a player is not here
        bool isPlayer = false;
        for (int i=0;i<getNbPlayers();i++)
        {
            QPoint playerBlock = getBlockPosition(players[i]->getX(), players[i]->getY());
            if (block.x() == playerBlock.x() && block.y() == playerBlock.y())
            {
                isPlayer = true;
                break;
            }
        }

        //check if a bonus is here
        if(getTileBonus(block.x(), block.y()) != 0)
            break;

        if(isPlayer)
            break;
        BombServer *b = addBomb(playerId, block.x(), block.y());
        if( !b )
            break;
        newBombs << b;
    }
    return newBombs;
}

BombServer* MapServer::addBomb(int playerId, int squareX, int squareY)
{
    BlockMapProperty::BlockType type = getType(squareX,squareY);
    if(getTileBomb(squareX, squareY) != 0 || type != BlockMapProperty::empty)
        return NULL;

	// add the bomb
    QPoint coords = getCenterCoordForBlock(squareX, squareY);
	BombServer *newBomb = new BombServer(playerId,
                                         coords.x(),
                                         coords.y(),
                                         DEFAULT_BOMB_DURATION,
                                         players[playerId]->getFlameLength(),
                                         players[playerId]->getRemoteBonus(),
                                         players[playerId]->getOilBonus(),
                                         PixelToBlock(getBlockSize()) );
    if ( !newBomb->getIsRC() && (qrand() % 100) == 0 )// 1% of dud bomb
    {
        newBomb->setDudBomb();
        newBomb->setDuration(DEFAULT_BOMB_DURATION * DEFAULT_BOMB_DUD_MALUS);
    }
    Map<PlayerServer,BombServer,mapStyle>::addBomb(newBomb);
    connect(newBomb,
            SIGNAL(sigTileChanged(qint16,qint8,qint8,qint8,qint8)),
            this,
            SLOT(slotBombTileChanged(qint16,qint8,qint8,qint8,qint8)));
    connect(newBomb,
            SIGNAL(sigFlyingBombChange(qint16)),
            this,
            SLOT(slotFlyingBombChange(qint16)));
	//qDebug() << " MapServer> AddBomb : " << getBombList().size() << " BOMBS !!! x: " << squareX
    //         << " y: " << squareY << " bombId: " << newBomb->getBombId();
	players[playerId]->decBombsAvailable();
	return newBomb;
}


Flame* MapServer::explosion(BombServer* b)
{
    removeBomb(b->getBombId());
	Flame *f = new Flame(b->getPlayerId(), heartBeat, 20);
	f->addDetonatedBomb(b->getBombId());

    QPoint block = getBlockPosition(b->getX(), b->getY());
    QPoint tempPoint = QPoint(block.x(), block.y());
	propagateFlame(*f, tempPoint, b->getRange());

    players[b->getPlayerId()]->incBombsAvailable();
	delete b;
	addFlame(f);
	//flames.append(f);

	//qDebug()<<"BOOM !";
	return f;
}

void MapServer::propagateFlame(Flame & f, const QPoint & p, int range)
{
	if (!f.getFlamePositions().contains(p))
	{
		f.addFlame(p.x(),p.y());
//		for (int i=0;i<getNbPlayers();i++)
//		{
//			qint16 x,y;
//			getPlayerPosition(i,x,y);
//			QPoint block = getBlockPosition(x,y);
//			if (p.x()==block.x() && p.y()==block.y())
//				qDebug() << "player "<<f.getPlayerId()<<" pwned player "<<i;
//		}
	}

	directedFlameProgagation(f,p,planeDirUp,range);
	directedFlameProgagation(f,p,planeDirDown,range);
	directedFlameProgagation(f,p,planeDirLeft,range);
	directedFlameProgagation(f,p,planeDirRight,range);
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

        BombServer* b = getTileBomb(pTemp.x(), pTemp.y());
        if(b != 0) {
            removeBomb(b->getBombId());
            f.addDetonatedBomb(b->getBombId());
            propagateFlame(f, pTemp, b->getRange());
            players[b->getPlayerId()]->incBombsAvailable();
            delete b;
            return;
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

void MapServer::doPlayerDeath(PlayerServer* playerN, int killedBy)
{
    playerN->setIsAlive(false);
    killedPlayers.append(killedPlayer(playerN->getId(), killedBy));
    //check if the player has detonators bomb on the field
    foreach(BombServer * b, getBombList())
    {
        if (b->getPlayerId() == playerN->getId())
        {
            //qDebug("dead player has a detonator bomb");
            b->unsetRC();
            b->setDuration(DEFAULT_BOMB_DURATION);
        }
    }

    //respawn bonus of this player
    int capacity;
    if(playerN->getOilBonus())
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_OIL;
    if(playerN->getMultibombBonus())
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_MULTIBOMB;
    if(playerN->getThrowbombBonus())
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_THROW_GLOVE;
    if(playerN->getBoxingGloveBonus())
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_BOXING_GLOVE;
    if(playerN->hasRemoteBonus())
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_REMOTE;
    if(playerN->getKickBonus())
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_KICK;
    capacity = playerN->getFlameLength() - DEFAULT_BOMB_RANGE;
    while(capacity-- > 0)
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_FLAME;
    capacity = playerN->getMaxNumberOfBombs() - DEFAULT_BOMB_CAPACITY;
    while(capacity-- > 0)
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_BOMB;
    capacity = playerN->getMoveDistance() - MOVE_STEP;
    while(capacity-- > 0)
        bonusToSpawn << (Bonus::Bonus_t)Bonus::BONUS_FASTER;
}

bool MapServer::checkPlayerInFlames(PlayerServer* playerN, const QPoint& playerBlock)
{
    const QSet<Flame*> flameList = getTileFlames(playerBlock.x(), playerBlock.y());
    if(flameList.size() > 0) {
        qint32 oldestTimestamp = 0x80000000;
        Flame* f = 0;

        // we look for the oldest explosion thanks to the timestamp
        // if there are several explosions at once, we keep the first one we find
        // this is fair as long as flameList is unordered
        foreach(Flame* flameN, flameList)
            if(flameN->getTimestamp() > oldestTimestamp) {
                oldestTimestamp = flameN->getTimestamp();
                f = flameN;
            }

        doPlayerDeath(playerN, f->getPlayerId());
        return true;
    }

    return false;
}

void MapServer::exchangePlayersPositions(PlayerServer *p1, PlayerServer *p2)
{
    int x = p1->getX();
    int y = p1->getY();
    p1->setX(p2->getX());
    p1->setY(p2->getY());
    p2->setX(x);
    p2->setY(y);
}

void MapServer::exchangePlayersPositions()
{
    for(int i = 0; i < players.size(); i += 2)
    {
        if(i == players.size() - 1)
            exchangePlayersPositions(players[0],players[i]);
        else
            exchangePlayersPositions(players[i],players[i+1]);
    }
}

void MapServer::applyWalkwayToPlayer(PlayerServer* playerN, globalDirection playerDirection)
{
    QPoint actPoint = getBlockPosition(playerN->getX(), playerN->getY());
    if(getOption(actPoint.x(),actPoint.y()) == BlockMapProperty::mov_walk)
    {
        //qDebug() << "player" << playerN->getId() << "on moving walkway";
        // no effect if directions are othogonal (cf AB)
        globalDirection walkDir = getOptionDirection(actPoint.x(),actPoint.y());
        if( (walkDir == dirUp || walkDir == dirDown)
            && (playerDirection == dirRight || playerDirection == dirLeft) )
            return;
        if( (walkDir == dirRight || walkDir == dirLeft)
            && (playerDirection == dirUp || playerDirection == dirDown) )
            return;
        bool dummy;
        tryMovePlayer(playerN->getId(), walkDir, WALKWAY_SPEED,dummy);
    }
}

void MapServer::checkPlayerSurroundings(PlayerServer* playerN) {
    QPoint actPoint = getBlockPosition(playerN->getX(), playerN->getY());

    // check whether the player threw himself in a flame
    if(checkPlayerInFlames(playerN, actPoint))
        return;

    // TODO check for other player close by for disease

    Bonus* pickedUpBonus = removeBonus(actPoint.x(), actPoint.y());
    if(pickedUpBonus) {
        // TODO does the code belong to MapServer or to PlayerServer?
        if(pickedUpBonus->getType() == Bonus::BONUS_RANDOM)
        {
            int randomDraw = static_cast<int>((static_cast<double>(qrand()) / RAND_MAX) * (NB_BONUS-2));
            Q_ASSERT(randomDraw != Bonus::BONUS_RANDOM && randomDraw != Bonus::BONUS_NONE);
            //qDebug() << "random bonus" << randomDraw;
            pickedUpBonus->setType(static_cast<Bonus::Bonus_t>(randomDraw));
        }
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
                sickness sick = static_cast<sickness> (random);
                if(sick == SICK_SHUFFLE)
                {
                    //qDebug() << "sick shuffle";
                    exchangePlayersPositions();
                }
                else
                    playerN->setSickness(sick);
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
    if(playerN->getOnTeleport() )
    {
        int id;
        if(!blockContainsTeleport(actPoint.x(),actPoint.y(),id))
        {
            //qDebug("player moved out of the teleport");
            playerN->setOnTeleport(false);
        }
    }
    else
    {
        int id;
        if(blockContainsTeleport(actPoint.x(),actPoint.y(),id))
        {
            int x_next_tp, y_next_tp;
            //qDebug() << "player moved in the teleport" << id;
            playerN->setOnTeleport(true);
            getNextTeleportPosition(id, x_next_tp, y_next_tp);
            QPoint actPoint(x_next_tp * getBlockSize() + getBlockSize() / 2, y_next_tp * getBlockSize() + getBlockSize() / 2);
            setPlayerPosition(playerN->getId(),actPoint.x(),actPoint.y());
            checkPlayerInFlames(playerN, actPoint);
            return;
        }
    }
}

Bonus* MapServer::removeBonus(qint8 x, qint8 y) {
    Bonus* remBonus = getTileBonus(x, y);
    if(remBonus == 0)
        return 0;
    setTileBonus(x, y, 0);

    createdBonus.removeAll(remBonus);
    removedBonus.append(Point<qint8>(x, y));

    return remBonus;
}

void MapServer::brokenBlockRemoved(int x, int y) {
    int randomDraw = static_cast<int>((static_cast<double>(qrand()) / RAND_MAX) * BONUS_TABLE_LENGTH);

    if(debugMode)
        randomDraw = randomDraw % ( (NB_BONUS-1)*16 - 1);

    Bonus::Bonus_t result = bonusTable[randomDraw];
    if(result != Bonus::BONUS_NONE) {
        Bonus* newBonus = new Bonus(result, x, y);
        setTileBonus(x, y, newBonus);
        createdBonus.append(newBonus);
    }
}

bool MapServer::shrinkMap()
{
    int count = 0;
    do
    {
        if(shrinkDirection == planeDirRight)
        {
            if(shrink.x() < shrinkLimitDown.x()-1)
            {
                shrink.setX(shrink.x()+1);
                break;
            }
            else
            {
                shrinkDirection = planeDirDown;
                shrinkLimitUp.setY(shrinkLimitUp.y()+1);
            }
        }
        else if(shrinkDirection == planeDirDown)
        {
            if(shrink.y() < shrinkLimitDown.y()-1)
            {
                shrink.setY(shrink.y()+1);
                break;
            }
            else
            {
                shrinkDirection = planeDirLeft;
                shrinkLimitDown.setX(shrinkLimitDown.x()-1);
            }
        }
        else if(shrinkDirection == planeDirLeft)
        {
            if(shrink.x() > shrinkLimitUp.x())
            {
                shrink.setX(shrink.x()-1);
                break;
            }
            else
            {
                shrinkDirection = planeDirUp;
                shrinkLimitDown.setY(shrinkLimitDown.y()-1);
            }
        }
        else if(shrinkDirection == planeDirUp)
        {
            if(shrink.y() > shrinkLimitUp.y())
            {
                shrink.setY(shrink.y()-1);
                break;
            }
            else
            {
                shrinkDirection = planeDirRight;
                shrinkLimitUp.setX(shrinkLimitUp.x()+1);
            }
        }
        count++;
    }while(count < 5);//test all directions

    if(count <= 4)
    {
       setType(BlockMapProperty::wall, shrink.x(), shrink.y());
       //check for players, bombs and bonuses
       // Remove the bonus here
       delete removeBonus(shrink.x(), shrink.y());
       //Kill players on this block
       foreach(PlayerServer* playerN, players) {
           if(playerN->getIsAlive()) {
               QPoint playerBlock = getBlockPosition(playerN->getX(), playerN->getY());
               if(playerBlock.x() == shrink.x() && playerBlock.y() == shrink.y())
                   doPlayerDeath(playerN,-1);
           }
       }
       //if there is a bomb, it must explode
       BombServer* b = getTileBomb(shrink.x(), shrink.y());
       if(b)
       {
           b->unsetRC();
           b->setDuration(0);
       }
       return true;
    }
    else
        qDebug() << "Map completly skrinked";
    //shouldn't happen in a normal game
    return false;
}

void MapServer::startHeartBeat(qint32 startValue, int intervals) {
    shrinkLimitUp.setX(0);
    shrinkLimitUp.setY(0);
    shrinkLimitDown.setX(width);
    shrinkLimitDown.setY(height);
	setHeartBeat(startValue);
	timerHeartBeat = new QTimer;
    connect(timerHeartBeat, SIGNAL(timeout()), this, SLOT(newHeartBeat()));
	timerHeartBeat->start(intervals);
}

void MapServer::newHeartBeat() {
	heartBeat--;
    if(heartBeat % 1000 == 0)
        qDebug() << "send Hearbeat #" << heartBeat;

    killedPlayers.clear();
    createdBonus.clear();
    removedBonus.clear();

	QByteArray updateArray;
	QDataStream updateOut(&updateArray,QIODevice::WriteOnly | QIODevice::Truncate);
	updateOut << heartBeat;

	//Add a new block if necessary, or (0,0)
	qint16 x = 0,y = 0;
	bool brick = true;
    if(heartBeat < 0 && heartBeat % SHRINKING_SPEED == 0)
    {
        if(!shrinkMap())
            emit sigWinner(-1); //shouldn't happen in a normal game
        //qDebug() << "shrinking" << shrink.x() << shrink.y();
        x = shrink.x();
        y = shrink.y();
        brick = false; //wall

    }
    else if(spawningBlocks && ( heartBeat % (SPAWNING_BLOCK_INTERVAL * 1000/HEARTBEAT) == 0) )
    {
        if(getRandomEmptyPosition(x,y))
            setType(BlockMapProperty::brick, x, y);
    }
    updateOut << brick << x << y;

	// start by cleaning flames
	QList<qint16> cleanList;
    QMap<Flame::flameId_t, Flame*> flameList = getFlameList();
    foreach(Flame* flameN, flameList) {
		flameN->decreaseLifeSpan();
		if(flameN->isFinished()) {
			cleanList.append(flameN->getFlameId());
			removeFlame(flameN->getFlameId());
		}
	}
	updateOut << cleanList;

	// now let each alive player lay a bomb, then move
    QList<BombServer*> newBombs;
    QList<BombServer*> flyingBombs;

    foreach(PlayerServer* playerN, players)
    {
        if(playerN->getIsAlive())
        {
            if(playerN->getLayingBomb() || playerN->getSickness() == SICK_DIARRHEA)
            {
                QPoint plBlock = getBlockPosition(playerN->getX(),playerN->getY());
                BombServer *currentBomb = getTileBomb(plBlock.x(), plBlock.y());
                if(playerN->getIsBombAvailable())
                {
                    if(!currentBomb)
                    {
                        BombServer* newBomb = addBomb(playerN->getId(), plBlock.x(), plBlock.y());
                        if(newBomb != NULL)
                            newBombs << newBomb;
                    }
                    else if(playerN->getLayingBomb() && playerN->getMultibombBonus())
                        newBombs << addBombMultiple(playerN->getId());
                }
                if(currentBomb && playerN->getThrowbombBonus())
                {
                    throwBomb(currentBomb, playerN->getHeading(),3);
                    flyingBombs << currentBomb;
                }
                playerN->clearLayingBomb();
            }
            if(playerN->getDirection() != -1)
            {
                bool againstWall = false;
                globalDirection move = movePlayer(playerN->getId(), playerN->getDirection(), playerN->getMoveDistance(), againstWall);
                playerN->setMove(true);
                playerN->setDirection(-1);
                /* if the player couldn't move because of a block there is no need to apply the walkway because :
                 * - the walkway does not apply if its direction is orthogonal to the player direction
                 * - the walkway must not get the player backwark if he is blocked by a wall
                 */
                if(!againstWall && move != dirNone)
                    applyWalkwayToPlayer(playerN,move);
            }
            else
                applyWalkwayToPlayer(playerN,dirNone);

            /* Check if the player wants to throw a bomb */
            if(playerN->getBoxingGloveBonus() && playerN->getOptKey())
            {
                QPoint block = getBlockPosition(playerN->getX(),playerN->getY());
                getNextBlock(block.x(), block.y(), block.rx(), block.ry(), playerN->getHeading());
                BombServer *bomb = getTileBomb(block.x(), block.y());
                if(bomb)
                {
                    throwBomb(bomb, playerN->getHeading(),3);
                    flyingBombs << bomb;
                }

            }
            checkPlayerSurroundings(playerN);
        }
    }


	// serialize all players information
	updateOut << static_cast<qint8>(players.size());
	foreach(PlayerServer* playerN, players) {
		updateOut << *playerN;
		//update Player infos
		playerN->decreaseDuration();
		playerN->setMove(false);
	}

	// serialize the new bombs
	updateOut << static_cast<qint8>(newBombs.size());
	foreach(BombServer* bombN, newBombs) {
		updateOut << *bombN;
	}

    // move the bombs
    QList<BombServer*> movingBombs;
    foreach(BombServer* bombN, getBombList()) {
        bool hasMoved = false;

        // conveyor belt has no effect on a rolling bomb, check both cases separately!
        if(bombN->getDirection() != dirNone) {
            if(bombN->getFlying())
            {
                hasMoved = moveFlyingBomb(bombN);
                if(hasMoved &&  bombN->getFlying())
                    flyingBombs << bombN;
            }
            else
            {
                hasMoved = tryMoveBomb(bombN, dirNone);
                if(!bombN->getHasMoved())
                {
                    if(hasMoved)
                        bombN->setHasMoved(true);
                    else
                        bombN->setDirection(dirNone);
                }
            }
        }
        else
        {
            QPoint sourceBlock = getBlockPosition(bombN->getX(), bombN->getY());
            if(getOption(sourceBlock.x(), sourceBlock.y()) == BlockMapProperty::mov_walk)
                hasMoved = tryMoveBomb(bombN, getOptionDirection(sourceBlock.x(), sourceBlock.y()));
        }

        if(hasMoved) {
            QPoint neighBlock = getOverlappingBlockPosition(bombN->getX(), bombN->getY());
            delete removeBonus(neighBlock.x(), neighBlock.y());
            movingBombs.append(bombN);
        }
    }

    // serialize the moving bombs
	updateOut << static_cast<qint8>(movingBombs.size());
	foreach(BombServer* bombN, movingBombs) {
		updateOut << bombN->getBombId() << bombN->getX() << bombN->getY();
	}

	// serialize the new flying bombs
	updateOut << static_cast<qint8>(flyingBombs.size());
	foreach(BombServer* bombN, flyingBombs) {
	    QPoint p = bombN->getDestination();
	    updateOut << bombN->getBombId() << static_cast<qint16>(p.x()) << static_cast<qint16>(p.y()) << bombN->getFlHeartbeat();
	}
	// then decrease each bomb's counter
    foreach(BombServer* bombN, getBombList())
    {
       if(!bombN->getFlying())
           bombN->decreaseLifeSpan();
    }

	// now we check which bombs must explode
	// FIXME : because a bomb exploding can trigger several other ones
	//           we must restart from the beginning of the list every time
	//           to ensure the iterator is still valid
    // FIXME if the bomb moves into the flames, we probably want to change the bomb's owner into the flame's owner
	QList<Flame*> explodeList;
	QMap<BombServer::bombId_t, BombServer*>::const_iterator itBomb = getBombList().begin();
	while(itBomb != getBombList().end()) {
        BombServer* b = *itBomb;
		if( b->mustExplode() ||
            ( b->getIsRC() && players[b->getPlayerId()]->getOptKey() ) ||
            getTileFlames(b->getTileX(), b->getTileY()).size() > 0
          )
        {
			explodeList.append(explosion(*itBomb));
			itBomb = getBombList().begin();
		}
		else
			++itBomb;
	}

	// serialize the list of explosions
	updateOut << static_cast<qint8>(explodeList.size());
	foreach(Flame* flameN, explodeList)
		updateOut << *flameN;

    // check which players just were killed by the explosions
    foreach(PlayerServer* playerN, players)
    {
        if(playerN->getIsAlive())
        {
            QPoint playerBlock = getBlockPosition(playerN->getX(), playerN->getY());
            checkPlayerInFlames(playerN, playerBlock);
        }
    }

    // create bonuses if needed
    while(!bonusToSpawn.empty())
    {
        Bonus::Bonus_t bonus_type = bonusToSpawn.takeFirst();
        qint16 x,y;
        //qDebug() << "bonus to spawn" << bonus_type;
        if(getRandomEmptyPosition(x,y))
        {
            Bonus* newBonus = new Bonus(bonus_type, x, y);
            setTileBonus(x, y, newBonus);
            createdBonus.append(newBonus);
        }
        else
        {
            qDebug() << "spawn bonus, plus de place";
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
        timerHeartBeat->stop();
        emit sigWinner(-1);
        return;
    }

    if((! debugMode) && playersAlive.size() == 1) {
        timerHeartBeat->stop();
        emit sigWinner(playersAlive.first());
        return;
    }

    // TODO time over
}

void MapServer::slotBombTileChanged(qint16 bombId, qint8 oldx, qint8 oldy, qint8 newx, qint8 newy)
{
    Map<PlayerServer,BombServer,mapStyle>::slotBombTileChanged(bombId, oldx, oldy, newx, newy);
}

void MapServer::slotFlyingBombChange(qint16 bombId)
{
    Map<PlayerServer,BombServer,mapStyle>::slotFlyingBombChange(bombId);
}

globalDirection MapServer::reverseDirection(globalDirection initialDir)
{
    switch(initialDir) {
        case dirLeft:   return dirRight;
        case dirRight:  return dirLeft;
        case dirUp:     return dirDown;
        case dirDown:   return dirUp;
        default:        Q_ASSERT(false);
    }

    return dirNone;     // we should never arrive here, but it removes a GCC warning
}

//We only need this operator in the server, and it must match the >> operator in MapClient
QDataStream& operator<<(QDataStream& out, const mapStyle& ms)
{
    out << ms.name << static_cast<quint8> (ms.option);
    return out;
}
