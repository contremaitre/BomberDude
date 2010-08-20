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

GameField::GameField(int nb_x, int nb_y, int s)
{
    scene = new QGraphicsScene;
    caseList.init(nb_x,nb_y,s);
    for(int i = 0; i < nb_x; i++)
    {
        for(int j = 0; j < nb_y; j++)
        {
            QGraphicsCaseItem *m_case = caseList.getCase(i,j);
            scene->addItem(m_case->getItem());
        }
    }
    view = new QGraphicsView(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->show();

}

void GameField::movePlayer(int player, int position)
{
    caseList.movePlayer(player,position);
}

void GameField::getEventFilter(QObject *obj)
{
    scene->installEventFilter(obj);
}

const Map *GameField::getMap()
{
    return caseList.getMap();
}

GameField::~GameField()
{
    delete view;
}

