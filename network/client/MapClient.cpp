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
        	emit sigMovePlayer(id, playerN.getX(), playerN.getY());
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
	for(qint8 i = 0; i < nbMovingBombs; i++) {
		qint16 bombId, nx, ny;
		updateIn >> bombId >> nx >> ny;
        BombClient* b = getBomb(bombId);
        if(b != 0) {
            b->setX(nx);
            b->setY(ny);
            b->setFlying(false);
        }
		emit sigMovedBomb(bombId);
	}

    qint8 nbFlyingBombs;
    updateIn >> nbFlyingBombs;
    for(qint8 i = 0; i < nbFlyingBombs; i++) {
        qint16 bombId, nx, ny;
        qint32 hb;
        updateIn >> bombId >> nx >> ny >> hb;
        BombClient* b = getBomb(bombId);
        if(b != 0) {
            QPoint dest(nx,ny);
            b->setFlying(true);
            b->setDestination(dest);
            b->setFlHeartbeat(hb);
        }
        emit sigFlyingBomb(bombId);
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
        //replace detonators bombs of this player with standard bombs
        foreach (BombClient *b, getBombList())
        {
            if(b->getPlayerId() == plId && b->getIsRC())
            {
                b->unsetRC();
                emit sigRemoveBombRC(b->getBombId());
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
    qDebug() << "map client, received style" << ms.name;
    return in;
}
