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

#include "Serverd.h"
#include "constant.h"

Serverd::Serverd(int port, int mapSize, int blockSize)
{
    server = new NetServer(port);
    server->setMapSize(MAP_SIZE,MAP_SIZE,BLOCK_SIZE);
}

Serverd::Serverd(int port, const char *mapFile)
{
    server = new NetServer(port);
    server->setMapFile(mapFile);
}

void Serverd::launch()
{
    //We start the game as soon as a player is connecter to the server
    //others players can join later
    //the way the game is launched will be changed later.
    connect(server, SIGNAL(allPlayersLeft()), this, SLOT(allPlayersLeft()));
    qDebug() << "launch server";
    server->start();
}

void Serverd::allPlayersLeft()
{
    qDebug() << "allPlayersLeft";
    //server->loadMap();
    //server->createRandomMap(MAP_SIZE,MAP_SIZE,BLOCK_SIZE);

}

Serverd::~Serverd()
{
    server->quit();
    server->wait();
    delete server;
    delete map;
}

