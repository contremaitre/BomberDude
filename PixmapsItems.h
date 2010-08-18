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
    QPixmap bomberman[MAX_NB_PLAYER];
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

