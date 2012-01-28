/*
    Copyright (C) 2011 Thomas Kister

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

#include "BombServer.h"


qint16 BombServer::index=1;


BombServer::BombServer(qint8 playerId,
                       qint16 x,
                       qint16 y,
                       int duration,
                       int range,
                       bool remote,
                       bool oil,
                       PixelToBlock ptb) :
    Bomb<PixelToBlock>(index++, playerId, x, y, remote, ptb),
    duration(duration),
    range(range),
    hasOil(oil),
    direction(dirNone),
    hasMoved(false)
{}

BombServer::~BombServer()
{}
