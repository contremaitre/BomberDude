#ifndef BLOCKMAPPROPERTY_H
#define BLOCKMAPPROPERTY_H

class BlockMapProperty
{
public:
    //must be kept in sync in PixmapsItem
    enum BlockType {wall,brick,bomb,player,empty};
    enum Option {none,o_bomb};

    BlockMapProperty();
    ~BlockMapProperty();

    void setType(BlockType);
    BlockType getType();
private:
    BlockType type;
    Option option;

};

#endif

