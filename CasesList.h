#ifndef CASELIST_H
#define CASELIST_H
#include "QGraphicsCaseItem.h"
#include "Map.h"
#include "PixmapsItems.h"
#include <QObject>

class CasesList : public QObject
{
  Q_OBJECT
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
    QGraphicsCaseItem *getCase(int);
    QGraphicsCaseItem *getCase(int, int);
    void init(int,int,int);
    void getPlayerPosition(int, int &, int &);
    bool movePlayer(int, int, int);
    int getWidth();
    int getHeight();

private slots:
    void blockChanged(int);

signals:
    void pixmapChanged(int);

};

#endif

