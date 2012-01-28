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

#ifndef QTB_TILE_H
#define QTB_TILE_H


#include <QMap>
#include <QSet>

#include "Player.h"
#include "Bomb.h"
#include "Flame.h"
#include "Bonus.h"


template<typename P, typename B>
struct Tile {
    Tile() :
        withBomb(0),
        withBonus(0)
    {}

    QMap<int, P*> withPlayers;
    B* withBomb;
    QSet<Flame*> withFlames;
    Bonus* withBonus;
};


#endif // QTB_TILE_H
