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
    bool eventFilter(QObject *obj, QEvent *event);
    /**
     * Move a player 
     * \param id player index
     * \param direction
     */
    bool move(int,int);

public:
    GameField(int,int,int);
    ~GameField();
};

#endif

