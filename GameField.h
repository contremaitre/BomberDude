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

#ifndef PLATEAU_H
#define PLATEAU_H
#include <QObject>
#include <QGraphicsView>
#include "CasesList.h"
#include "Bomb.h"

class GameField : public QObject
{
  Q_OBJECT

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    CasesList caseList;
    QList <Bomb> bombList;
    bool eventFilter(QObject *obj, QEvent *event);
    /**
     * Move a player 
     * \param id player index
     * \param direction
     */
    bool move(int,int);

    /**
     * A player is dropping a bomb
     */
    void dropBomb(int plId);

private slots:
    void pixmapChanged(int);

public:
    GameField(int,int,int);
    ~GameField();
};

#endif

