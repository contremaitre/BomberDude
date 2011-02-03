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

#include "PixmapsItems.h"

PixmapsItems::PixmapsItems()
{
    width = height = 0;
}

PixmapsItems::~PixmapsItems()
{
}

void PixmapsItems::init(int w, int h)
{
    width = w;
    height = h;
    loadAll();
}

void PixmapsItems::loadAll()
{
    for(int i = 0; i < 10; i++) {
        QString pixPath = "pictures/tux_0" + QString::number(i) + ".png";
        bomberman[i] = QPixmap(pixPath).scaled(QSizeF(width,height).toSize());
    }

    addBonusPixMap(Bonus::BONUS_FLAME,"pictures/bonus_flame.png");
    addBonusPixMap(Bonus::BONUS_BOMB,"pictures/bonus_bomb.png");
    addBonusPixMap(Bonus::BONUS_OIL,"pictures/bonus_oil.png");
    addBonusPixMap(Bonus::BONUS_DISEASE,"pictures/bonus_disease.png");
    addBonusPixMap(Bonus::BONUS_KICK,"pictures/bonus_kick.png");
    addBonusPixMap(Bonus::BONUS_FASTER,"pictures/bonus_faster.png");

    burnt = QPixmap("pictures/tux_burn.png").scaled(QSizeF(width,height).toSize());

    addBlockPixMap(BlockMapProperty::brick,"pictures/brick.png");
    addBlockPixMap(BlockMapProperty::wall,"pictures/wall.png");
    addBlockPixMap(BlockMapProperty::bomb,"pictures/bomb.png");
    addBlockPixMap(BlockMapProperty::flame,"pictures/explosion.png");
    addBlockPixMap(BlockMapProperty::broken,"pictures/broken.png");
}

void PixmapsItems::addBlockPixMap(BlockMapProperty::BlockType type, const char *name)
{
    block_pixmaps_t tmp_block;
    tmp_block.pixmap = QPixmap(name).scaled(QSizeF(width,height).toSize());
    tmp_block.type = type;
    block_pixmaps.push_back(tmp_block);
}

void PixmapsItems::addBonusPixMap(Bonus::Bonus_t type, const char *name)
{
    option_pixmaps_t tmp_block;
    tmp_block.pixmap = QPixmap(name).scaled(QSizeF(width,height).toSize());
    tmp_block.type = type;
    option_pixmaps.push_back(tmp_block);
}

const QPixmap& PixmapsItems::getPixmap(BlockMapProperty::BlockType type)
{
    for (int i = 0; i < block_pixmaps.size(); ++i) {
        if (block_pixmaps.at(i).type == type)
            return block_pixmaps.at(i).pixmap;
    }
    //todo could this case be avoided ?
    return none;
}

const QPixmap& PixmapsItems::getPixmap(Bonus::Bonus_t type)
{
    for (int i = 0; i < option_pixmaps.size(); ++i) {
        if (option_pixmaps.at(i).type == type)
            return option_pixmaps.at(i).pixmap;
    }
    Q_ASSERT(false);
    return none;
}

const QPixmap& PixmapsItems::getPixmap(int player)
{
    //for now, the max number of differents player graphic is 10
    Q_ASSERT(player < 10);
    return bomberman[player];
}

