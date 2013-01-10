/*
    Copyright (C) 2010,2011,2012 Sébastien Escudier

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

#include "MapClient.h"

bool operator<(const QPoint& p1, const QPoint& p2) {
    if(p1.x() == p2.x())
        return p1.y() < p2.y();
    return p1.x() < p2.x();
}


void MapClient::updateMap(QByteArray& updateBlock) {
	// FIXME this method should not be called before the map has been setup,
	// i.e. the calling method which is a slot should not be connected before
	// I'll leave that as an exercise to whoever fixes a bit the workflow of
	// the client :p

	QDataStream updateIn(updateBlock);

	qint32 heartBeat;
	updateIn >> heartBeat;
	setHeartBeat(heartBeat);
	if(heartBeat % 500 == 0)
	    qDebug() << "Received heartbeat " << heartBeat;

	//new block
	qint16 x,y;
	bool brick;
	updateIn >> brick >> x >> y;
	//qDebug() << x << y << "shrinked";
	if(x > 0 && y > 0)
	    setType(brick ? BlockMapProperty::brick : BlockMapProperty::wall, x, y);

	QList<qint16> cleanList;
	updateIn >> cleanList;
	foreach(qint16 flameId, cleanList) {
		removeFlame(flameId);
        emit sigRemoveFlame(flameId);
    }

	qint8 playerListSize;
	updateIn >> playerListSize;
//	qDebug() << playerListSize << " players received";
	for(qint8 i = 0; i < playerListSize; i++) {
		Player playerN;
		updateIn >> playerN;
		//qDebug() << "player " << i << "sick" << playerN.getIsSick();
		int id = playerN.getId();
        if(playerN.getIsAlive()){
        	setPlayerPosition(id, playerN.getX(), playerN.getY());
        	players[id]->setHeading(playerN.getHeading());
        	if(playerN.getMove())
        	    emit sigMovePlayer(id, playerN.getX(), playerN.getY(), playerN.getHeading());
        	if(setPlayerSickness(id, playerN.getIsSick()))
        	    emit sigPlayerSickChanged(id, playerN.getIsSick());
        }
	}

	qint8 newBombsListSize;
	updateIn >> newBombsListSize;
//	qDebug() << newBombsListSize << " new bombs received";
	for(qint8 i = 0; i < newBombsListSize; i++) {
		BombClient* bombN = new BombClient(PixelToBlock(getBlockSize()));
		updateIn >> *bombN;
		addBomb(bombN);
		emit sigAddBomb(bombN->getBombId());
	}

    qint8 nbMovingBombs;
    updateIn >> nbMovingBombs;
    /* We need to store the moving bombs in a list to check if they are still flying
     * before emiting the signal */
    QList<int> movingBombs;
	for(qint8 i = 0; i < nbMovingBombs; i++) {
		qint16 bombId, nx, ny;
		updateIn >> bombId >> nx >> ny;
        BombClient* b = getBomb(bombId);
        if(b != NULL) {
            b->setX(nx);
            b->setY(ny);
            b->setFlying(false); //if the bomb is still flying, it will be set back right after.
        }
        movingBombs << bombId;
	}

    qint8 nbFlyingBombs;
    updateIn >> nbFlyingBombs;
    for(qint8 i = 0; i < nbFlyingBombs; i++) {
        qint16 bombId, nx, ny;
        qint32 hb;
        updateIn >> bombId >> nx >> ny >> hb;
        BombClient* b = getBomb(bombId);
        if(b != NULL) {
            QPoint dest(nx,ny);
            b->setFlying(true);
            b->setDestination(dest);
            b->setFlHeartbeat(hb);
        }
        emit sigFlyingBomb(bombId, heartBeat);
    }
    while(!movingBombs.empty())
        emit sigMovedBomb(movingBombs.takeFirst());
	qint8 nbExplosions;
	updateIn >> nbExplosions;
	for(qint8 i = 0; i < nbExplosions; i++) {
		Flame* f = new Flame();
		updateIn >> *f;

		QList<qint16> detonatedBombs = f->getDetonatedBombs();
        foreach(qint16 bombId, detonatedBombs)
		{
            emit sigRemoveBomb(bombId);
			removeBomb(bombId);
		}

        foreach(QPoint point, f->getFlamePositions())
            emit sigAddFlame(f->getFlameId(), point.x(), point.y());

        QSet<QPoint> brokenBlocks = f->getBrokenBlocks();
        foreach(QPoint bb, brokenBlocks)
        {
            //qDebug() << "GameArena brokenblock";
            int i = bb.x();
            int j = bb.y();
            setType(BlockMapProperty::broken, i, j);
            //emit sigBlockChanged(i,j);
        }
		addFlame(f);
	}

    // list of players killed during this hearbeat
    QList<MapClient::killedPlayer> killedPlayers;
    updateIn >> killedPlayers;

    foreach(MapClient::killedPlayer frag, killedPlayers) {
        int plId = frag.first;
        sigKillPlayer(plId);
    }

    quint8 nbCreatedBonus;
    updateIn >> nbCreatedBonus;
    for(quint8 i = 0; i < nbCreatedBonus; i++) {
        Bonus b;
        updateIn >> b;
        emit sigAddBonus(b.getType(), b.getX(), b.getY());
    }

    quint8 nbRemovedBonus;
    updateIn >> nbRemovedBonus;
    for(quint8 i = 0; i < nbRemovedBonus; i++) {
        qint8 x, y;
        updateIn >> x >> y;
        emit sigRemoveBonus(x, y);
    }

    quint8 nbRemovedRC;
    updateIn >> nbRemovedRC;
    for(quint8 i = 0; i < nbRemovedRC; i++) {
        qint16 id;
        updateIn >> id;
        qDebug() << "client removed RC" << id;
        getBomb(id)->unsetRC();
        emit sigRemoveBombRC(id);
    }
}

void MapClient::addStyle(const mapStyle &style)
{
    styles << style;
}
const QList<mapStyle> *MapClient::getStyles() const
{
    return &styles;
}


const BombClient& MapClient::getRefBomb(qint16 bombId)
{
    BombClient* b = getBomb(bombId);
    Q_ASSERT(b != 0);
    return *b;
}


QDataStream& operator>>(QDataStream& in, mapStyle& ms)
{
    quint8 opt;
    in >> ms.name >> opt;
    ms.option = static_cast<BlockMapProperty::BlockOption>(opt);
    //qDebug() << "map client, received style" << ms.name;
    return in;
}
