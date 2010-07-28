#include "Map.h"
#include "stdlib.h" //temporaraire (NULL)
#include <time.h> //qrand seed
#include <QtGlobal>

Map::Map()
{
    block_list = NULL;
    setDim(0,0);
}

Map::Map(int w, int h)
{
    block_list = NULL;
    setDim(w,h);
}

void Map::Init()
{
    delete[] block_list;
    if( width * height > 0 )
    {
        block_list = new BlockMapProperty[width*height];
    }
    else
        block_list = NULL;
}

void Map::setDim(int w, int h)
{
    width = w;
    height = h;
    Init();
}

void Map::loadRandom()
{
    qsrand(time(NULL));
    for(int w = 1; w < width-1; w++)
    {
        for(int h = 1; h < height-1; h++)
        {
            //printf("%d,%d\n",w,h);
            //randomly add wall or bricks or nothing
            double d = (double)qrand() / RAND_MAX;
            if(d < 0.1)
                block_list[h*width+w].setType(BlockMapProperty::wall);
            else if(d < 0.4)
                block_list[h*width+w].setType(BlockMapProperty::brick);
            else
                block_list[h*width+w].setType(BlockMapProperty::empty);
        }
    }
    //add walls on the map sides
    for(int w = 0; w < width; w++)
    {
        //printf("%d,%d\n",w,0);
        //printf("%d,%d\n",w,0);
        block_list[w].setType(BlockMapProperty::wall);
        block_list[(height-1)*width+w].setType(BlockMapProperty::wall);
    }
    for(int h = 1; h < height-1; h++)
    {
        block_list[h*width].setType(BlockMapProperty::wall);
        block_list[h*width+width-1].setType(BlockMapProperty::wall);
    }
}

BlockMapProperty::BlockType Map::getType(int w,int h)
{
    return block_list[h*width+w].getType();
}

int Map::getWidth()
{
    return width;
}

int Map::getHeight()
{
    return height;
}


Map::~Map()
{
    delete[] block_list;
}

