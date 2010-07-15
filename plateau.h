#ifndef PLATEAU_H
#define PLATEAU_H
#include <QGraphicsView>
#include "QGraphicsCaseItem.h"

class plateau : public QObject
{
  Q_OBJECT

private:
    QGraphicsCaseItem caseItem;
    QGraphicsView *view;
    QGraphicsScene *scene;
public:
    plateau();
};

#endif
