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

Serverd::Serverd(int port,
                 const char *adminPasswd,
                 int mapSize,
                 int blockSize,
                 bool debugMode,
                 bool startedFromGui)
{
    server = new NetServer(port, adminPasswd, debugMode, startedFromGui);
    connect(server,SIGNAL(sigQuit()), this, SLOT(slotQuit()), Qt::QueuedConnection);
    connect(server,SIGNAL(allPlayersLeft()), this, SLOT(allPlayersLeft()));
}

void Serverd::launch()
{
    server->start();
}

void Serverd::allPlayersLeft()
{
    qDebug() << "allPlayersLeft";
    server->restart();
}

void Serverd::slotQuit()
{
    qDebug("Serverd::slotQuit()");
    emit sigQuit();
}

Serverd::~Serverd()
{
    delete server;
}

