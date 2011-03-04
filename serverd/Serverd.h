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

/**
 *
 * This is where the Network server, maps and graphic can interact
 * Player input keys from the scene are captured here
 */


#ifndef QTB_SERVERD_H
#define QTB_SERVERD_H


#include <QObject>

#include "NetServer.h"
#include "MapServer.h"


class Serverd : public QObject
{
  Q_OBJECT

public:
    Serverd(int port,
            const char *adminPasswd,
            int mapSize,
            int blockSize,
            bool debugMode,
            bool startedFromGui);
//    Serverd(int port, const char *adminPasswd, const char *mapFile);
    ~Serverd();
    void launch();

private:
    void init(int port, const char *adminPasswd, bool debugMode, bool startedFromGui);
    NetServer *server;

private slots:
    void allPlayersLeft();
    void slotQuit();

signals:
    void sigQuit();
};


#endif // QTB_SERVERD_H
