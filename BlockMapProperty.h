#ifndef BLOCKMAPPROPERTY_H
#define BLOCKMAPPROPERTY_H

#define MAX_NB_PLAYER 4

class BlockMapProperty
{
public:
    //must be kept in sync in PixmapsItem
    enum BlockType {wall,brick,player,empty};
    enum Option {none,bomb};

    BlockMapProperty();
    ~BlockMapProperty();

    void setType(BlockType);
    BlockType getType();
private:
    BlockType type;
    Option option;

};

#endif

