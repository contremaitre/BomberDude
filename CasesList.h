#ifndef CASELIST_H
#define CASELIST_H
#include "QGraphicsCaseItem.h"
#include "Map.h"
#include "PixmapsItems.h"

class CasesList
{
private:
    Map map;
    PixmapsItems pixmaps;
    QGraphicsCaseItem **casesItem;
    void initCase(int, int);
    void loadPixMaps();
    int width;
    int height;
    int caseSize;
public:
    CasesList();
    ~CasesList();
    QGraphicsCaseItem *getCase(int, int);
    void init(int,int);
};

#endif

