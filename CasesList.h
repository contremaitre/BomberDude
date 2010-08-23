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
#include "QGraphicsCaseItem.h"
#include "Map.h"
#include "PixmapsItems.h"
#include <QObject>

class CasesList : public QObject
{
  Q_OBJECT
private:
    Map map;
    PixmapsItems pixmaps;
    QGraphicsCaseItem **casesItem;
    void initCase(int, int);
    void init();
    void loadPixMaps();
    int width;
    int height;
    int caseSize;
public:
    CasesList(int caseSize);
    ~CasesList();
    QGraphicsCaseItem *getCase(int);
    QGraphicsCaseItem *getCase(int, int);
    void createRandomMap(int w, int h);
    int getWidth();
    int getHeight();
    void movePlayer(int player, int position);
    const Map *getMap();
    void setMap(const Map *);

private slots:
    void blockChanged(int);

signals:
    void pixmapChanged(int);

};

#endif

