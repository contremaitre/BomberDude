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
#include "NetServer.h"
#include "constant.h"
#include "MapServer.h"

Serverd::Serverd(int port, int mapSize, int blockSize)
{
    server = new NetServer(port);
    server->createRandomMap(MAP_SIZE,MAP_SIZE,BLOCK_SIZE);
}

void Serverd::launch()
{
    //We start the game as soon as a player is connecter to the server
    //others players can join later (but they may miss movement me can make before they join
    //the way the game is launched will be changed later.
    connect(server,SIGNAL(newPlayer()),this,SLOT(slotStart()),Qt::QueuedConnection);
    qDebug() << "launch server";
    server->start();
}

void Serverd::slotStart()
{
    qDebug() << "client connecter, start";
    server->assignNumberToPlayers();
}

Serverd::~Serverd()
{
    server->quit();
    server->wait();
    delete server;
    delete map;
}

