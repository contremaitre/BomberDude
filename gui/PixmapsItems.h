/*
    Copyright (C) 2010,2011 Sébastien Escudier

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

#ifndef QTB_PIXMAPSITEMS_H
#define QTB_PIXMAPSITEMS_H


#include <QPixmap>
#include <QMap>

#include "BlockMapProperty.h"
#include "Bonus.h"
#include "constant.h"

/**
 * For now, we load one picture for each block type. And this is done manualy
 * In the future, we will have something like a xml file with an association of block type and pictures
 */
class PixmapsItems
{
private:
    struct block_pixmaps_t {QPixmap pixmap; BlockMapProperty::BlockType type;};
    struct bonus_pixmaps_t {QPixmap pixmap; Bonus::Bonus_t type;};
    QList <block_pixmaps_t> block_pixmaps;
    QList <bonus_pixmaps_t> bonus_pixmaps;

    struct option_pixmaps_t {
        bool operator<(const option_pixmaps_t& rhs) const {
            if(option != rhs.option)
                return option < rhs.option;
            else
                return dir < rhs.dir;
        }

        BlockMapProperty::BlockOption option;
        globalDirection dir;
    };
    QMap<option_pixmaps_t, QPixmap> option_pixmaps;

    QPixmap flame;
    QPixmap burnt;
    QPixmap sick;
    QPixmap option_unknown;
    QPixmap none;
    QPixmap bomb;
    QPixmap bombrc;
    QPixmap bombdud;
    void loadAll();
    void addBlockPixMap(BlockMapProperty::BlockType, const char *);
    void addOptionPixMap(BlockMapProperty::BlockOption, const char *);
    void addOptionPixMapRotate(BlockMapProperty::BlockOption, const char *);
    void addBonusPixMap(Bonus::Bonus_t, const char *);
    //width and height to scale the pixmaps to the good size
    int width, height;
public:
    PixmapsItems();
    ~PixmapsItems();
    void init(int,int);
    const QPixmap& getPixmap(BlockMapProperty::BlockOption, globalDirection = dirNone);
    const QPixmap& getPixmap(BlockMapProperty::BlockType);
    const QPixmap& getPixmap(Bonus::Bonus_t);
    const QPixmap& getPixmapFlame() const                   { return flame; }
    const QPixmap& getPixmapBurnt() const                   { return burnt; }
    const QPixmap& getPixmapSick() const                    { return sick; }
    const QPixmap& getPixmapBomb() const                    { return bomb; }
    const QPixmap& getPixmapBombrc() const                  { return bombrc; }
    const QPixmap& getPixmapBombDud() const                 { return bombdud; }
};


#endif // QTB_PIXMAPSITEMS_H
