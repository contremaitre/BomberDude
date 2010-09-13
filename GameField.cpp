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

#include "GameField.h"
#include <QMainWindow>

GameField::GameField(QMainWindow *mainw, int s) : blockList(s)
{
    scene = new QGraphicsScene;
    mainWindow = mainw;
    view = NULL;
}

void GameField::createRandomMap(int width, int height)
{
    blockList.createRandomMap(width, height);
}

void GameField::createGraphics()
{
    for(int i = 0; i < blockList.getWidth(); i++)
    {
        for(int j = 0; j < blockList.getHeight(); j++)
        {
            QGraphicsCaseItem *m_case = blockList.getCase(i,j);
            scene->addItem(m_case->getItem());
        }
    }
    for(int i = 0 ; i < blockList.getNbPlayers(); i++)
    {
            QGraphicsCaseItem *m_case = blockList.getPlayer(i);
            scene->addItem(m_case->getItem());
    }
    view = new QGraphicsView(mainWindow);
    int size = blockList.getCaseSize() * (blockList.getWidth()+1);
    mainWindow->setMinimumSize(size,size);
    view->setMinimumSize(size,size);
    view->setScene(scene);
    view->show();
}

void GameField::movePlayer(int player, int x, int y)
{
    blockList.movePlayer(player, x, y);
}

void GameField::getEventFilter(QObject *obj)
{
    scene->installEventFilter(obj);
}

void GameField::setMap(const Map* map)
{
    blockList.setMap(map);
}

const Map *GameField::getMap()
{
    return blockList.getMap();
}

GameField::~GameField()
{
    delete view;
}

