#ifndef QGRAPHICSCASEITEM_H
#define QGRAPHICSCASEITEM_H

#include <QGraphicsPixmapItem>

enum caseType
{
    VOID,
    BOMBERMAN,
    BOMB,
    
};

class QGraphicsCaseItem : public QGraphicsPixmapItem
{
private:
    int x,y,size;
public:
    QGraphicsCaseItem();
    QGraphicsCaseItem(int,int,int);
    void setPos(int, int, int);
    void setItem(caseType);
    void setItem(const QPixmap &);
    QGraphicsPixmapItem *getItem();
    //QGraphicsPixmapItem pixItem;
};

#endif

