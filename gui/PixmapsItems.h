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

#ifndef PIXMAPSITEMS_H
#define PIXMAPSITEMS_H

#include <QPixmap>
#include "BlockMapProperty.h"
#include "constant.h"
/**
 * For now, we load one picture for each block type. And this is done manualy
 * In the future, we will have something like a xml file with an association of block type and pictures
 */
class PixmapsItems
{
private:
    struct block_pixmaps_t{QPixmap pixmap; BlockMapProperty::BlockType type;};
    struct option_pixmaps_t{QPixmap pixmap; BlockMapProperty::Option type;};
    QList <block_pixmaps_t> block_pixmaps;
    QList <option_pixmaps_t> option_pixmaps;
    QPixmap bomberman[1]; //For now, only one player pixmap is available
    QPixmap none;
    void loadAll();
    //width and height to scale the pixmaps to the good size
    int width, height;
public:
    PixmapsItems();
    ~PixmapsItems();
    void init(int,int);
    QPixmap getPixmap(BlockMapProperty::BlockType);
    QPixmap getPixmap(int);
    QPixmap getPixmap(BlockMapProperty::Option);
};

#endif

