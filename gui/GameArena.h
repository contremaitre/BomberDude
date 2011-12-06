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

#ifndef QTB_GAMEARENA_H
#define QTB_GAMEARENA_H


#include <QObject>
#include <QList>

#include "QGraphicsSquareItem.h"
#include "MapClient.h"
#include "PixmapsItems.h"

class QGraphicsView;

// necessary to use QPoint in a QMap
bool operator<(const QPoint&, const QPoint&);

struct playersItem_t
{
    QGraphicsSquareItem item;
    bool sick;
    bool itemSick;
};

class GameArena : public QObject
{
  Q_OBJECT
private:
    QGraphicsView *graphicView;
    QGraphicsScene *scene;
    MapClient* map;
    PixmapsItems pixmaps;
    QGraphicsSquareItem **squaresItem;
    playersItem_t *playersItem;
    QList<QGraphicsSquareItem*> burntPlayers;
    QList<QGraphicsSquareItem*> optionsItems;
    QMap<QPoint, QGraphicsItem*> bonus;
    QMap<int, QGraphicsSquareItem*> bombs;
    QMap<int, QGraphicsSquareItem*> flames;

    QGraphicsSquareItem *getCase(int);
    QGraphicsSquareItem *getCase(int, int);

    QGraphicsSimpleTextItem* textHurryUp1, *textHurryUp2;

    void initCase(int, int);
    int width;
    int height;
    int maxNbPlayers;
    int squareSize;
    int timeInSeconds;

public:
    GameArena(int blockSize);
    ~GameArena();
    //QGraphicsSquareItem *getPlayer(int);
    void createGraphics();
    void setGraphicView(QGraphicsView *);
    void getEventFilter(QObject *obj);
    int getWidth();
    int getHeight();
    int getCaseSize() const;
    void addFlame(Flame* flame);
    void removeFlame(int flameId);
    void removeBomb(qint16 bombId);
//	void updateMap(QByteArray& updateBlock);
    int getNbPlayers() const;
    const MapClient *getMap();
    void setMap(MapClient *);
    void clear();

private slots:
    void movePlayer(int player, int x, int y);
    void slotPlayerSickChanged(int player, bool sick);
    void blockChanged(int);
	void blockChanged(int x, int y);
    void slotHearbeatUpdated(qint32 value);
    void removeBurnt();
    void killPlayer(int);
    void slotAddBonus(Bonus::Bonus_t type, qint16 x, qint16 y);
    void slotRemoveBonus(qint16 x, qint16 y);
    void slotAddBomb(int id);
    void slotMovedBomb(int id);
    void slotFlyingBomb(int id);
    void slotRemoveBomb(int id);
    void slotRemoveBombRC(int id);
    void slotRemoveHurryUp();
    void slotAddFlame(int id, qint16 x, qint16 y);
    void slotRemoveFlame(int id);

public slots:
    void slotMapWinner(qint8 playerId);

signals:
    void sigTimeUpdated(int valueInSeconds);
    void sigNewPlayerGraphic(int,const QPixmap &);
};


#endif // QTB_GAMEARENA_H
