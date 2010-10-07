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

#ifndef CASELIST_H
#define CASELIST_H
#include "QGraphicsSquareItem.h"
#include "Map.h"
#include "PixmapsItems.h"
#include <QObject>
#include <QList>

class QMainWindow;
class QGraphicsView;

class GameArena : public QObject
{
  Q_OBJECT
private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    QMainWindow *mainWindow;
    Map map;
    PixmapsItems pixmaps;
    QGraphicsSquareItem **squaresItem;
    QGraphicsSquareItem **playersItem;
    QMap<Bomb*,QGraphicsSquareItem*> bombsItem;
    QHash<int,QList<QGraphicsSquareItem*>* > flamesItem;
    void initCase(int, int);
    void init();
    void loadPixMaps();
    int width;
    int height;
    int squareSize;
public:
    GameArena(QMainWindow *, int blockSize);
    ~GameArena();
    QGraphicsSquareItem *getCase(int);
    QGraphicsSquareItem *getCase(int, int);
    QGraphicsSquareItem *getPlayer(int);
    void createGraphics();
    void createRandomMap(int w, int h);
    void getEventFilter(QObject *obj);
    int getWidth();
    int getHeight();
    int getCaseSize() const;
    void movePlayer(int player, int x, int y);
    void addBomb(int player, int x, int y, int bombId);
    void addFlame(Flame& flame);
    void removeFlame(int flameId);
    void removeBomb(int bombId);
    int getNbPlayers() const;
    const Map *getMap();
    void setMap(const Map *);

private slots:
    void blockChanged(int);

signals:
    void pixmapChanged(int);
};

#endif

