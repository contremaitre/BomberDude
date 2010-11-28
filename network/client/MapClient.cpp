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
		int id = playerN.getId();
        if(playerN.getIsAlive()){
        	setPlayerPosition(id, playerN.getX(), playerN.getY());
        	emit sigMovePlayer(id, playerN.getX(), playerN.getY());
        }

	}

	qint8 newBombsListSize;
	updateIn >> newBombsListSize;
//	qDebug() << newBombsListSize << " new bombs received";
	for(qint8 i = 0; i < newBombsListSize; i++) {
		Bomb bombN;
		updateIn >> bombN;
		addBomb(bombN.playerId, bombN.x, bombN.y, bombN.bombId);
	}

	qint8 nbExplosions;
	updateIn >> nbExplosions;
	for(qint8 i = 0; i < nbExplosions; i++) {
		Flame* f = new Flame();
		updateIn >> *f;

		QList<qint16>::const_iterator itBomb = f->getFirstDetonatedBomb();
		for( ; itBomb != f->getLastDetonatedBomb(); ++itBomb)
			removeBomb(*itBomb);
        QSet<QPoint>::const_iterator itBrBlock = f->getFirstBrokenBlock();
        for( ; itBrBlock != f->getLastBrokenBlock(); ++itBrBlock)
        {
            //qDebug() << "GameArena brokenblock";
            int i = itBrBlock->x();
            int j = itBrBlock->y();
            setType(BlockMapProperty::broken, i, j);
            emit sigBlockChanged(i,j);
        }
		addFlame(f);
	}

    // list of players killed during this hearbeat
    QList<MapClient::killedPlayer> killedPlayers;
    updateIn >> killedPlayers;

    foreach(MapClient::killedPlayer frag, killedPlayers) {
        sigKillPlayer(frag.first);
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
        qint16 x, y;
        updateIn >> x >> y;
        emit sigRemoveBonus(x, y);
    }
}

