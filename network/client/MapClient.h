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

#ifndef QTB_MAPCLIENT_H
#define QTB_MAPCLIENT_H


#include "Map.h"
#include "Player.h"
#include "BombClient.h"
#include "Flame.h"
#include "Bonus.h"


// necessary to use QPoint in a QMap
bool operator<(const QPoint&, const QPoint&);


class QGraphicsItem;

struct mapStyle
{
    QString name;
    BlockMapProperty::BlockOption option;
};

class MapClient : public Map<Player,BombClient,mapStyle>
{
    Q_OBJECT
public :
    void updateMap(QByteArray& updateBlock);
    void addStyle(const mapStyle &style);
    const QList<mapStyle> *getStyles() const;

    const BombClient& getRefBomb(qint16 bombId);

private:
    QMap<QPoint, QGraphicsItem*> bonus;

signals:
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
    void sigHeartbeatUpdated(qint32 value);

    void sigMovePlayer(int player, int x, int y, int heading);
    void sigPlayerSickChanged(int player, bool sick);
    void sigKillPlayer(int);
    void sigAddBonus(Bonus::Bonus_t type, qint16 x, qint16 y);
    void sigRemoveBonus(qint16 x, qint16 y);
    void sigAddBomb(int);
    void sigMovedBomb(int);
    void sigFlyingBomb(int,qint32);
    void sigRemoveBomb(int);
    void sigRemoveBombRC(int);
    void sigAddFlame(int id, qint16 x, qint16 y);
    void sigRemoveFlame(int id);
};

QDataStream& operator>>(QDataStream& in, mapStyle& ms);


#endif // QTB_MAPCLIENT_H
