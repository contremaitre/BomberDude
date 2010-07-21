#include "CasesList.h"

CasesList::CasesList()
{
    width = height = 0;
    caseSize = 20;
    casesItem = NULL;
    loadPixMaps();
}

void CasesList::init(int w, int h)
{
    width = w;
    height = h;
    casesItem = new QGraphicsCaseItem*[w * h];
    for(int i = 0; i < w; i++)
    {
        for(int j = 0; j < h; j++)
        {
            initCase(i,j);
            getCase(i,j)->setItem(brique);
        }
    }
    getCase(0,0)->setItem(bombermans[0]);
    getCase(w-1,h-1)->setItem(bombermans[0]);
}

void CasesList::loadPixMaps()
{
    QPixmap p("pictures/tux.png");
    bombermans[0] = p.scaled(QSizeF(caseSize,caseSize).toSize());
    QPixmap p2("pictures/brique.jpg");
    //p.load("pictures/brique.jpg");
    brique = p2.scaled(QSizeF(caseSize,caseSize).toSize());
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
