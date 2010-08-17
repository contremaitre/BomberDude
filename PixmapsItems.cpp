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
    if(type == BlockMapProperty::player)
        return getPixmap(0); //we don't implement multiplayer pixmaps yet
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


