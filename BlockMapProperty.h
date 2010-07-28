#ifndef BLOCKMAPPROPERTY_H
#define BLOCKMAPPROPERTY_H


class BlockMapProperty
{
public:
    //must be kept in sync in PixmapsItem
    enum BlockType {wall,brick,empty};
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

