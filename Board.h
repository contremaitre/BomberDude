#ifndef PLATEAU_H
#define PLATEAU_H
#include <QGraphicsView>
#include "CasesList.h"
class Board : public QObject
{
  Q_OBJECT

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    CasesList caseList;
public:
    Board(int,int);
};

#endif
