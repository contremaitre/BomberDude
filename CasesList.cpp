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

#include <QDebug>
#include "CasesList.h"

CasesList::CasesList(int s)
{
    width = height = 0;
    caseSize = s;
    casesItem = NULL;
    playersItem = NULL;
    connect(&map,SIGNAL(blockChanged(int)),this,SLOT(blockChanged(int)));
    loadPixMaps();
}

void CasesList::loadPixMaps()
{
    pixmaps.init(caseSize, caseSize);
}

void CasesList::init()
{
    width = map.getWidth();
    height = map.getHeight();
    delete[] casesItem;
    delete[] playersItem;
    casesItem = new QGraphicsCaseItem*[width * height];
    playersItem = new QGraphicsCaseItem*[map.getMaxNbPlayers()];
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            initCase(i,j);
            getCase(i,j)->setItem(pixmaps.getPixmap(map.getType(i,j)));
        }
    }
    qint16 x,y;
    for(int i = 0; i < map.getMaxNbPlayers(); i++)
    {
        map.getPlayerPosition(i,x,y);
        qDebug() << "player" << i << "postition" << x << y;
        playersItem[i] = new QGraphicsCaseItem(x-caseSize/2,y-caseSize/2,caseSize);
        playersItem[i]->setItem(pixmaps.getPixmap(i));
    }
}

void CasesList::setMap(const Map *map)
{
    this->map = *map;
    init();
}

void CasesList::createRandomMap(int w, int h)
{
    map.setDim(w,h,caseSize);
    map.loadRandom();
    init();
}


void CasesList::movePlayer(int player, int x, int y)
{
    map.setPlayerPosition(player, x, y);
    playersItem[player]->setPos(x-caseSize/2,y-caseSize/2,caseSize);
}

int CasesList::getCaseSize() const
{
    return caseSize;
}

void CasesList::initCase(int i, int j)
{
    int x_a = i*caseSize;
    int y_a = j*caseSize;
    casesItem[j*width+i] = new QGraphicsCaseItem(x_a,y_a,caseSize);
}

void CasesList::blockChanged(int pos)
{
    getCase(pos)->setItem(pixmaps.getPixmap(map.getType(pos)));
    emit pixmapChanged(pos);
}

const Map *CasesList::getMap()
{
    return &map;
}

int CasesList::getWidth()
{
    return width;
}

int CasesList::getHeight()
{
    return height;
}

int CasesList::getNbPlayers() const
{
    return map.getMaxNbPlayers();
}

QGraphicsCaseItem *CasesList::getCase(int i, int j)
{
    return casesItem[j*width+i];
}

QGraphicsCaseItem *CasesList::getCase(int pos)
{
    return casesItem[pos];
}

QGraphicsCaseItem *CasesList::getPlayer(int id)
{
    return playersItem[id];
}

CasesList::~CasesList()
{
    if(casesItem)
    {
        for(int i = 0; i < width*height; i++)
            delete casesItem[i];
        delete []casesItem;
    }
    if(playersItem)
    {
        for(int i = 0; i < map.getMaxNbPlayers(); i++)
            delete playersItem[i];
        delete[] playersItem;
    }
}

