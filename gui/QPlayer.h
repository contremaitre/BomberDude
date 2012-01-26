#ifndef QPLAYER_H
#define QPLAYER_H

#include "QAnimateditem.h"
#include <QDebug>
#include "../common/constant.h"

enum PlayerAction{
    actionWalkingN=0,
    actionWalkingS,
    actionWalkingE,
    actionWalkingW,
    actionStayingStillN,
    actionStayingStillS,
    actionStayingStillE,
    actionStayingStillW,
    actionBurning //warning, when adding an action burning should stay the last action as it is used to reference the size of PlayerAction
};


class QPlayer : public QAnimatedItem
{
private:
    QList<QPixmap*>* walkingN;
    QList<QPixmap*>* walkingS;
    QList<QPixmap*>* walkingE;
    QList<QPixmap*>* walkingW;
    QList<QPixmap*>* stayingStillN;
    QList<QPixmap*>* stayingStillS;
    QList<QPixmap*>* stayingStillE;
    QList<QPixmap*>* stayingStillW;
    QList<QPixmap*>* burning;
    bool sick;
    int id;
    globalDirection currentDirection;
    int stayStillCount;
    bool bStayStill;

public:

    QPlayer(int);
    ~QPlayer();

    static void loadPixs();
    static void unloadPixs();
    static QList< QList<QPixmap*>**> allPix ;
    void walk(const globalDirection);
    void stayStill(const globalDirection);
    void burn();
    void setSick(const bool);
    void nextFrame();
    const QPixmap *getPlayerPixmap();
};


#endif // QPLAYER_H
