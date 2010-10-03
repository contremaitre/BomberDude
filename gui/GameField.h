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

/**
 *
 * This is the main graphic class
 * It will draw objects and animations
 *
 */

#ifndef GAMEFIELD_H
#define GAMEFIELD_H
#include <QObject>
#include <QGraphicsView>
#include "Area.h"
class QMainWindow;

class GameField : public QObject
{
  Q_OBJECT

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    Area area;
    QMainWindow *mainWindow;
public:
    GameField(QMainWindow *, int blockSize);
    ~GameField();
    void createRandomMap(int width, int height);
    void createGraphics();
    void getEventFilter(QObject *obj);
    void movePlayer(int player, int x, int y);
    void addBomb(int player, int x, int y, int bombId);
    void removeBomb(int x, int y);
    void setMap(const Map *);
    const Map *getMap();

public slots:
	void bombAdded(QGraphicsSquareItem*);
	void bombRemoved(QGraphicsSquareItem*);
};

#endif

