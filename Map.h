#ifndef MAP_H
#define MAP_H

#include "BlockMapProperty.h"
#include <QObject>
#include "constant.h"

/**
 * This class represent a bomberman game map
 * For now, the map is hardcoded.
 * Todo : add a load function, so we can load a map from a file
 */

class Map : public QObject
{
    Q_OBJECT
private:
    int width;
    int height;
    BlockMapProperty *block_list;
    int players_positions[MAX_NB_PLAYER];
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
    BlockMapProperty::BlockType getType(int);
    int *getPlayersPosition();
    void setPlayerPosition(int, int);

signals:
     void blockChanged(int pos);
};

#endif

