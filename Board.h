#ifndef PLATEAU_H
#define PLATEAU_H
#include <QGraphicsView>
#include "QGraphicsCaseItem.h"

class Board : public QObject
{
  Q_OBJECT

private:
    QGraphicsCaseItem caseItem;
    QGraphicsView *view;
    QGraphicsScene *scene;
public:
    Board();
};

#endif
