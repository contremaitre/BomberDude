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

CasesList::CasesList()
{
    width = height = 0;
    caseSize = 20;
    casesItem = NULL;
    connect(&map,SIGNAL(blockChanged(int)),this,SLOT(blockChanged(int)));
}

void CasesList::loadPixMaps()
{
    pixmaps.init(caseSize, caseSize);
}

void CasesList::init(int w, int h, int s)
{
    width = w;
    height = h;
    caseSize = s;
    loadPixMaps();
    map.setDim(w,h);
    map.loadRandom();
    casesItem = new QGraphicsCaseItem*[w * h];
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            initCase(i,j);
            getCase(i,j)->setItem(pixmaps.getPixmap(map.getType(i,j)));
        }
    }
    //getCase(1,1)->setItem(pixmaps.getPixmap(0));
    //getCase(w-2,h-2)->setItem(pixmaps.getPixmap(0));
}

bool CasesList::movePlayer(int id, int x, int y)
{
    if(x < 0 || x >= width || y < 0 || y >= height)
        return false;
    BlockMapProperty::BlockType type = map.getType(x,y);
    if( type == BlockMapProperty::player || type == BlockMapProperty::empty )
    {
        map.setPlayerPosition(id,y*width+x);
        return true;
    }
    return false;
}

void CasesList::initCase(int i, int j)
{
    int x_a = i*caseSize;
    int y_a = j*caseSize;
    //casesItem[j*width+i] = new QGraphicsCaseItem;
    //casesItem[j*width+i]->setPos(x_a,y_a,x_a+caseSize,y_a+caseSize);
    casesItem[j*width+i] = new QGraphicsCaseItem(x_a,y_a,caseSize);
}

void CasesList::getPlayerPosition(int pl, int &x, int &y)
{
    int *plPos = map.getPlayersPosition();
    x = plPos[pl] % width;
    y = plPos[pl] / width;
}

void CasesList::blockChanged(int pos)
{
    getCase(pos)->setItem(pixmaps.getPixmap(map.getType(pos)));
    emit pixmapChanged(pos);
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

