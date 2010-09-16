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


#ifndef SERVERD_H
#define SERVERD_H

#include <QObject>

class NetServer;
class Map;

class Serverd : public QObject
{
  Q_OBJECT

public:
    Serverd(int port, int mapSize, int blockSize);
    ~Serverd();
    void launch();

private:
    NetServer *server;
    Map *map;

private slots:
    void slotStart();
};

#endif

