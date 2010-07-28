#ifndef MAP_H
#define MAP_H

#include "BlockMapProperty.h"

/**
 * This class represent a bomberman game map
 * For now, the map is hardcoded.
 * Todo : add a load function, so we can load a map from a file
 */

class Map
{
private:
    int width;
    int height;
    BlockMapProperty *block_list;
    void Init();
public:
    Map();
    Map(int, int);
    ~Map();
    void setDim(int,int);
    void loadRandom();
    int getWidth();
    int getHeight();
    BlockMapProperty::BlockType getType(int,int);
};

#endif

