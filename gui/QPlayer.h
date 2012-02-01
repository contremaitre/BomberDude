/*
    Copyright (C) 2011,2012 Sébastien Escudier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    actionSick,
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

    /**
     * The sick anim will only be one pix (actualy a fake pix), and we will switch between
     * this pix and the normal anim.
     *  /!\ Warning TODO : The sick pix animation is complicated and should be improved /!\
     */
    QList<QPixmap*>* sickPixs;
    bool sick;
    int sickPreviousCurrentPix; //used when we swith between the sick anim and normal anim
    QList<QPixmap*>* sickPreviousCurrentAnim; //used when we swith between the sick anim and normal anim

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
