#ifndef PLATEAU_H
#define PLATEAU_H
#include <QGraphicsView>
#include "CasesList.h"

class GameField : public QObject
{
  Q_OBJECT

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    CasesList caseList;

public:
    GameField(int,int,int);
    ~GameField();
};

#endif

