#ifndef PLATEAU_H
#define PLATEAU_H
#include <QGraphicsView>

class plateau : public QObject
{
  Q_OBJECT

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
public:
    plateau();
};

#endif
