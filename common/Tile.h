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
