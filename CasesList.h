#ifndef CASELIST_H
#define CASELIST_H
#include "QGraphicsCaseItem.h"

class CasesList
{

private:
    QGraphicsCaseItem **casesItem;
    void initCase(int, int);
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
