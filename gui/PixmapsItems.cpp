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

    burnt = QPixmap("pictures/tux_burn.png").scaled(QSizeF(width,height).toSize());
    bonus_bomb = QPixmap("pictures/bonus_bomb.png").scaled(QSizeF(width,height).toSize());
    bonus_flame = QPixmap("pictures/bonus_flame.png").scaled(QSizeF(width,height).toSize());

    block_pixmaps_t tmp_block;
    
    QPixmap p2("pictures/brick.png");
    QPixmap ps2 = p2.scaled(QSizeF(width,height).toSize());
    tmp_block.pixmap = ps2;
    tmp_block.type = BlockMapProperty::brick;
    block_pixmaps.push_back(tmp_block);

    QPixmap p3("pictures/wall.png");
    QPixmap ps3 = p3.scaled(QSizeF(width,height).toSize());
    tmp_block.pixmap = ps3;
    tmp_block.type = BlockMapProperty::wall;
    block_pixmaps.push_back(tmp_block);

    QPixmap p4("pictures/bomb.png");
    QPixmap ps4 = p4.scaled(QSizeF(width,height).toSize());
    tmp_block.pixmap = ps4;
    tmp_block.type = BlockMapProperty::bomb;
    block_pixmaps.push_back(tmp_block);

    QPixmap p5("pictures/explosion.png");
    QPixmap ps5 = p5.scaled(QSizeF(width,height).toSize());
    tmp_block.pixmap = ps5;
    tmp_block.type = BlockMapProperty::flame;
    block_pixmaps.push_back(tmp_block);

    QPixmap p6("pictures/broken.png");
    QPixmap ps6 = p6.scaled(QSizeF(width,height).toSize());
    tmp_block.pixmap = ps6;
    tmp_block.type = BlockMapProperty::broken;
    block_pixmaps.push_back(tmp_block);
}

const QPixmap& PixmapsItems::getPixmap(BlockMapProperty::BlockType type)
{
    for (int i = 0; i < block_pixmaps.size(); ++i) {
        if (block_pixmaps.at(i).type == type)
            return block_pixmaps.at(i).pixmap;
    }
    return none;
}

const QPixmap& PixmapsItems::getPixmap(int player)
{
    //for now, the max number of differents player graphic is 10
    Q_ASSERT(player < 10);
    return bomberman[player];
}

const QPixmap& PixmapsItems::getPixmap(BlockMapProperty::Option type)
{
    for (int i = 0; i < block_pixmaps.size(); ++i) {
        if (option_pixmaps.at(i).type == type)
            return option_pixmaps.at(i).pixmap;
    }
    return none;
}
