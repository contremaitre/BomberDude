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
	if(heartBeat % 100 == 0)
	    qDebug() << "Received heartbeat " << heartBeat;

	QList<qint16> cleanList;
	updateIn >> cleanList;
	foreach(qint16 flameId, cleanList)
		removeFlame(flameId);

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
        	emit sigMovePlayer(id, playerN.getX(), playerN.getY());
        	if(setPlayerSickness(id, playerN.getIsSick()))
        	    emit sigPlayerSickChanged(id, playerN.getIsSick());
        }

	}

	qint8 newBombsListSize;
	updateIn >> newBombsListSize;
//	qDebug() << newBombsListSize << " new bombs received";
	for(qint8 i = 0; i < newBombsListSize; i++) {
		Bomb bombN;
		updateIn >> bombN;
		addBomb(bombN.playerId, bombN.x, bombN.y, bombN.bombId, bombN.remoteControlled);
		emit sigAddBomb(bombN.bombId);
	}

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
        //replace detonators bombs of this player with standard bombs
        foreach (Bomb *b, bombs)
        {
            if(b->playerId == plId && b->remoteControlled)
            {
                b->remoteControlled = false;
                emit sigRemoveBombRC(b->bombId);
            }
        }
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
}
