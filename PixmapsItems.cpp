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

void PixmapsItems::init(int w, int h)
{
    width = w;
    height = h;
    loadAll();
}

void PixmapsItems::loadAll()
{
//    enum BlockType {wall,brick,empty};
    block_pixmaps_t tmp_block;
    option_pixmaps_t tmp_option;
    QPixmap p("pictures/tux.png");
    QPixmap ps = p.scaled(QSizeF(width,height).toSize());
    for(int i = 0; i < MAX_NB_PLAYER; i++)
    {
        bomberman[i] = ps;
    }

    QPixmap p2("pictures/brique.jpg");
    QPixmap ps2 = p2.scaled(QSizeF(width,height).toSize());
    tmp_block.pixmap = ps2;
    tmp_block.type = BlockMapProperty::brick;
    block_pixmaps.push_back(tmp_block);

    QPixmap p3("pictures/stonewall.jpg");
    QPixmap ps3 = p3.scaled(QSizeF(width,height).toSize());
    tmp_block.pixmap = ps3;
    tmp_block.type = BlockMapProperty::wall;
    block_pixmaps.push_back(tmp_block);
}

QPixmap PixmapsItems::getPixmap(BlockMapProperty::BlockType type)
{
    for (int i = 0; i < block_pixmaps.size(); ++i) {
        if (block_pixmaps.at(i).type == type)
            return block_pixmaps.at(i).pixmap;
    }
    return none;
}

QPixmap PixmapsItems::getPixmap(int player)
{
    return bomberman[player];
}

QPixmap PixmapsItems::getPixmap(BlockMapProperty::Option type)
{
    for (int i = 0; i < block_pixmaps.size(); ++i) {
        if (option_pixmaps.at(i).type == type)
            return option_pixmaps.at(i).pixmap;
    }
    return none;
}

PixmapsItems::~PixmapsItems()
{
}


