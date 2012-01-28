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

#ifndef BOMBCLIENT_H
#define BOMBCLIENT_H


#include "Bomb.h"
#include "Map.h"


class BombClient : public Bomb<PixelToBlock>
{
    Q_OBJECT

public:
    BombClient(PixelToBlock ptc);
    virtual ~BombClient();

signals:
    void sigTileChanged(qint16 bombId, qint8 oldx, qint8 oldy, qint8 newx, qint8 newy);
    void sigFlyingBombChange(qint16 bombId);
};


#endif // BOMBCLIENT_H
