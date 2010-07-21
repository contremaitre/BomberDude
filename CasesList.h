#ifndef CASELIST_H
#define CASELIST_H
#include "QGraphicsCaseItem.h"

#define MAX_NB_PLAYER 4

class CasesList
{
private:
    QGraphicsCaseItem **casesItem;
    QPixmap bombermans[MAX_NB_PLAYER];
    QPixmap brique;
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
