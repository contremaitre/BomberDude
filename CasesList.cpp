#include "CasesList.h"

CasesList::CasesList()
{
    width = height = 0;
    caseSize = 20;
    casesItem = NULL;
    loadPixMaps();
}

void CasesList::loadPixMaps()
{
    pixmaps.init(caseSize, caseSize);
}

void CasesList::init(int w, int h)
{
    width = w;
    height = h;
    map.setDim(w,h);
    map.loadRandom();
    casesItem = new QGraphicsCaseItem*[w * h];
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            initCase(i,j);
            getCase(i,j)->setItem(pixmaps.getPixmap(map.getType(i,j)));
        }
    }
}

void CasesList::initCase(int i, int j)
{
    int x_a = i*caseSize;
    int y_a = j*caseSize;
    //casesItem[j*width+i] = new QGraphicsCaseItem;
    //casesItem[j*width+i]->setPos(x_a,y_a,x_a+caseSize,y_a+caseSize);
    casesItem[j*width+i] = new QGraphicsCaseItem(x_a,y_a,caseSize);
}


QGraphicsCaseItem *CasesList::getCase(int i, int j)
{
    return casesItem[j*width+i];
}

CasesList::~CasesList()
{
    if(casesItem)
    {
        for(int i = 0; i < width*height; i++)
            delete casesItem[i];
        delete []casesItem;
    }
}

