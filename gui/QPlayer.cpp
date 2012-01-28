/*
    Copyright (C) 2012 Sébastien Escudier

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

#include "QPlayer.h"
using namespace std;
QList<QList<QPixmap*>**> QPlayer::allPix=QList<QList<QPixmap*>**>();

QPlayer::QPlayer(int numero) {
    Q_ASSERT(!allPix.empty());
    id = numero;
    walkingN=QPlayer::allPix.at(numero)[actionWalkingN];
    walkingS=allPix.at(numero)[actionWalkingS];
    walkingE=allPix.at(numero)[actionWalkingE];
    walkingW=allPix.at(numero)[actionWalkingW];
    stayingStillN=allPix.at(numero)[actionStayingStillN];
    stayingStillS=allPix.at(numero)[actionStayingStillS];
    stayingStillE=allPix.at(numero)[actionStayingStillE];
    stayingStillW=allPix.at(numero)[actionStayingStillW];
    burning=allPix.at(numero)[actionBurning];

    currentDirection = dirLeft;
    currentPix = 0;
    stayStillCount = 0;
    stayStill(currentDirection);
}

void QPlayer::loadPixs()
{
    if(!allPix.empty())
        return;
    //todo number of players available shouldn't be hardcoded
    for (int i =0; i<6 ; i++)
    {
        QString baseName = "pictures/tux";
        QString s_i = QString::number(i);
        QList<QPixmap*>** animTable= new QList<QPixmap*>*[actionBurning+1];
        allPix.append(animTable);

        animTable[actionWalkingN]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionWalkingN], baseName + s_i + "1_up.png");
        QAnimatedItem::appendNewFrame(animTable[actionWalkingN], baseName + s_i + "2_up.png");

        animTable[actionWalkingS]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionWalkingS], baseName + s_i + "1_down.png");
        QAnimatedItem::appendNewFrame(animTable[actionWalkingS], baseName + s_i + "2_down.png");

        animTable[actionWalkingE]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionWalkingE], baseName + s_i + "1_left.png");
        QAnimatedItem::appendNewFrame(animTable[actionWalkingE], baseName + s_i + "2_left.png");

        animTable[actionWalkingW]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionWalkingW], baseName + s_i + "1_right.png");
        QAnimatedItem::appendNewFrame(animTable[actionWalkingW], baseName + s_i + "2_right.png");

        animTable[actionStayingStillN]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillN], baseName + s_i + "0_up.png");

        animTable[actionStayingStillS]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillS], baseName + s_i + "0_down.png");

        animTable[actionStayingStillE]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillE], baseName + s_i + "0_left.png");

        animTable[actionStayingStillW]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillW], baseName + s_i + "0_right.png");

        animTable[actionBurning]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionBurning], "pictures/tux_burn.png");

    }
}

void QPlayer::unloadPixs()
{
    while(!allPix.empty())
    {
        QList<QPixmap*> *first = *allPix.first();
        while(!first->empty())
        {
            delete first->takeFirst();
        }
        allPix.removeFirst();
    }
}

void QPlayer::walk(const globalDirection dir) {
    currentDirection = dir;
    switch (dir) {
    case dirUp :
        currentAnim=walkingN;
        break;
    case dirDown :
        currentAnim=walkingS;
        break;
    case dirLeft :
        currentAnim=walkingW;
        break;
    case dirRight :
        currentAnim=walkingE;
        break;

    }
    stayStillCount = 0;
    if(bStayStill)
    {
        bStayStill = false;
        currentPix = 0;
    }
}

void QPlayer::nextFrame()
{
    if(!bStayStill && ++stayStillCount > 3)
        stayStill(currentDirection);

    QAnimatedItem::nextFrame();
}

void QPlayer::stayStill(const globalDirection dir){
    switch (dir) {
    case dirUp :
        currentAnim=stayingStillN;
        break;
    case dirDown :
        currentAnim=stayingStillS;
        break;
    case dirLeft :
        currentAnim=stayingStillW;
        break;
    case dirRight :
        currentAnim=stayingStillE;
        break;

    }
    bStayStill = true;
    currentPix=0;
}


void QPlayer::burn(){
    currentAnim=burning;
    currentPix=0;

}
const QPixmap *QPlayer::getPlayerPixmap()
{
    return stayingStillS->first();

}

void QPlayer::setSick(const bool sick){
    this->sick=sick;
}

QPlayer::~QPlayer()
{
}
