#ifndef PLATEAU_H
#define PLATEAU_H
#include <QObject>
#include <QGraphicsView>
#include "CasesList.h"
#include "Bomb.h"

class GameField : public QObject
{
  Q_OBJECT

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    CasesList caseList;
    QList <Bomb> bombList;
    bool eventFilter(QObject *obj, QEvent *event);
    /**
     * Move a player 
     * \param id player index
     * \param direction
     */
    bool move(int,int);

    /**
     * A player is dropping a bomb
     */
    void dropBomb(int plId);

private slots:
    void pixmapChanged(int);

public:
    GameField(int,int,int);
    ~GameField();
};

#endif

