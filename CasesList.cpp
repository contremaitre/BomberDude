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

#include "CasesList.h"

CasesList::CasesList(int s)
{
    width = height = 0;
    caseSize = s;
    casesItem = NULL;
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
    casesItem = new QGraphicsCaseItem*[width * height];
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            initCase(i,j);
            getCase(i,j)->setItem(pixmaps.getPixmap(map.getType(i,j)));
        }
    }
}

void CasesList::setMap(const Map *map)
{
    this->map = *map;
    init();
}

void CasesList::createRandomMap(int w, int h)
{
    map.setDim(w,h);
    map.loadRandom();
    init();
}

void CasesList::movePlayer(int player, int position)
{
    map.setPlayerPosition(player,position);
}

void CasesList::initCase(int i, int j)
{
    int x_a = i*caseSize;
    int y_a = j*caseSize;
    //casesItem[j*width+i] = new QGraphicsCaseItem;
    //casesItem[j*width+i]->setPos(x_a,y_a,x_a+caseSize,y_a+caseSize);
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

QGraphicsCaseItem *CasesList::getCase(int i, int j)
{
    return casesItem[j*width+i];
}

QGraphicsCaseItem *CasesList::getCase(int pos)
{
    return casesItem[pos];
}

CasesList::~CasesList()
{
    if(casesItem)
    {
        for(int i = 0; i < width*height; i++)
            delete casesItem[i];
        delete []casesItem;
    }
}

