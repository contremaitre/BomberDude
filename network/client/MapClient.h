#ifndef QTB_MAPCLIENT_H
#define QTB_MAPCLIENT_H


#include "Map.h"
#include "Player.h"
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

class MapClient : public Map<Player,Bomb,mapStyle>
{
    Q_OBJECT
public :
    void updateMap(QByteArray& updateBlock);
    void addStyle(const mapStyle &style);
    const QList<mapStyle> *getStyles() const;
private:
    QMap<QPoint, QGraphicsItem*> bonus;

signals:
    void sigBlockChanged(int pos);
	void sigBlockChanged(int i, int j);
    void sigHeartbeatUpdated(qint32 value);

    void sigMovePlayer(int player, int x, int y);
    void sigPlayerSickChanged(int player, bool sick);
    void sigKillPlayer(int);
    void sigAddBonus(Bonus::Bonus_t type, qint16 x, qint16 y);
    void sigRemoveBonus(qint16 x, qint16 y);
    void sigAddBomb(int);
    void sigMovedBomb(int);
    void sigRemoveBomb(int);
    void sigRemoveBombRC(int);
};

QDataStream& operator>>(QDataStream& in, mapStyle& ms);


#endif // QTB_MAPCLIENT_H
