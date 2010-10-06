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

#include "GameField.h"
#include <QMainWindow>

GameField::GameField(QMainWindow *mainw, int s) : area(s)
{

	connect(&area,SIGNAL(bombAdded(QGraphicsSquareItem*)),this,SLOT(bombAdded(QGraphicsSquareItem*)));
	connect(&area,SIGNAL(bombRemoved(QGraphicsSquareItem*)),this,SLOT(bombRemoved(QGraphicsSquareItem*)));
	connect(&area,SIGNAL(flameAdded(QList<QGraphicsSquareItem*>&)),this,SLOT(flameAdded(QList<QGraphicsSquareItem*>&)));
	connect(&area,SIGNAL(flameRemoved(QList<QGraphicsSquareItem*>&)),this,SLOT(flameRemoved(QList<QGraphicsSquareItem*>&)));

	scene = new QGraphicsScene;
    mainWindow = mainw;
    view = NULL;
}

void GameField::createRandomMap(int width, int height)
{
    area.createRandomMap(width, height);
}

void GameField::createGraphics()
{
    for(int i = 0; i < area.getWidth(); i++)
    {
        for(int j = 0; j < area.getHeight(); j++)
        {
            QGraphicsSquareItem *m_case = area.getCase(i,j);
            scene->addItem(m_case->getItem());
        }
    }
    for(int i = 0 ; i < area.getNbPlayers(); i++)
    {
            QGraphicsSquareItem *m_case = area.getPlayer(i);
            scene->addItem(m_case->getItem());
    }
    view = new QGraphicsView(mainWindow);
    int size = area.getCaseSize() * (area.getWidth()+1);
    mainWindow->setMinimumSize(size,size);
    view->setMinimumSize(size,size);
    view->setScene(scene);
    view->show();
}

void GameField::addBomb(int player, int squareX, int squareY, int bombId)
{
    qDebug() << " addBomb() GameField " ; 
    area.addBomb(player, squareX, squareY, bombId);
}

void GameField::addFlame(Flame& flame)
{
	area.addFlame(flame);
}

void GameField::removeBomb(int bombId)
{
    //qDebug() << " removeBomb() GameField " ;
    area.removeBomb(bombId);
}

void GameField::removeFlame(int flameId){
	//qDebug() << " removeFlame() GameField " ;
	    area.removeFlame(flameId);
}

void GameField::movePlayer(int player, int x, int y)
{
    area.movePlayer(player, x, y);
}

void GameField::getEventFilter(QObject *obj)
{
    scene->installEventFilter(obj);
}

void GameField::setMap(const Map* map)
{
    area.setMap(map);
}

const Map *GameField::getMap()
{
    return area.getMap();
}

GameField::~GameField()
{
    delete view;
    //delete scene; todo crash ?
}

void GameField::bombAdded(QGraphicsSquareItem* bombItem){
	scene->addItem(bombItem);
	//qDebug() <<" bombAdded() GameField";
}

void GameField::bombRemoved(QGraphicsSquareItem* bombItem){
	//qDebug() <<" bombRemoved() GameField"<<bombItem;
	scene->removeItem(bombItem);
}

void GameField::flameAdded(QList<QGraphicsSquareItem*>& flamesItem){
	//qDebug()<< "GameField> flameAdded";
	foreach(QGraphicsSquareItem * item, flamesItem)
		{
			scene->addItem(item);
		}
}
void GameField::flameRemoved(QList<QGraphicsSquareItem*>& flamesItem){
	//qDebug()<< "GameField> flameRemoved"<<flamesItem.size();
	foreach(QGraphicsSquareItem * item, flamesItem)
		{
		//qDebug()<< "GameField> flameRemoved(2)";
			scene->removeItem(item);
		}

}

