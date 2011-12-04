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
    void initPixList();
    bool sick;

public:

    QPlayer();
    QPlayer(int);

    static QList< QList<QPixmap*>**> allPix ;
    void walk(const globalDirection);
    void stayStill(const globalDirection);
    void burn();
    void setSick(const bool);


};


#endif // QPLAYER_H
