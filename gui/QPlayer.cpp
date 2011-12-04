#include "QPlayer.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
using namespace std;
QList<QList<QPixmap*>**> QPlayer::allPix=QList<QList<QPixmap*>**>();


QPlayer::QPlayer(){
}

QPlayer::QPlayer(int numero) {

    if (allPix.size()==0)
        initPixList();

    walkingN=QPlayer::allPix.at(numero)[actionWalkingN];
    walkingS=allPix.at(numero)[actionWalkingS];
    walkingE=allPix.at(numero)[actionWalkingE];
    walkingW=allPix.at(numero)[actionWalkingW];
    stayingStillN=allPix.at(numero)[actionStayingStillN];
    stayingStillS=allPix.at(numero)[actionStayingStillS];
    stayingStillE=allPix.at(numero)[actionStayingStillE];
    stayingStillW=allPix.at(numero)[actionStayingStillW];
    burning=allPix.at(numero)[actionBurning];

}


void QPlayer::initPixList() {

    //todo number of players available shouldn't be hardcoded
    for (int i =0; i<6 ; i++)
    {
        stringstream ss (stringstream::in | stringstream::out);
        string s;
        QList<QPixmap*>** animTable= new QList<QPixmap*>*[actionBurning+1];
        allPix.append(animTable);

        animTable[actionWalkingN]=new QList<QPixmap*>();
        //QAnimatedItem::appendNewFrame(animTable[PlayerAction.walkingN], "pictures/tux"+i+"0_up.png");
        ss<<"pictures/tux"<<i<<"1_up.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingN], s.c_str());
        ss<<"pictures/tux"<<i<<"2_up.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingN], s.c_str());

        animTable[actionWalkingS]=new QList<QPixmap*>();
        //QAnimatedItem::appendNewFrame(animTable[PlayerAction.walkingS], "pictures/tux"+i+"0_down.png");
        ss<<"pictures/tux"<<i<<"1_down.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingS], s.c_str());
        ss<<"pictures/tux"<<i<<"2_down.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingS], s.c_str());

        animTable[actionWalkingE]=new QList<QPixmap*>();
        //QAnimatedItem::appendNewFrame(animTable[PlayerAction.walkingE], "pictures/tux"+i+"0_left.png");
        ss<<"pictures/tux"<<i<<"1_left.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingE], s.c_str());
        ss<<"pictures/tux"<<i<<"2_left.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingE], s.c_str());

        animTable[actionWalkingW]=new QList<QPixmap*>();
        //QAnimatedItem::appendNewFrame(animTable[PlayerAction.walkingW], "pictures/tux"+i+"0_right.png");
        ss<<"pictures/tux"<<i<<"1_right.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingW], s.c_str());
        ss<<"pictures/tux"<<i<<"2_right.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionWalkingW], s.c_str());

        animTable[actionStayingStillN]=new QList<QPixmap*>();
        ss<<"pictures/tux"<<i<<"0_up.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillN], s.c_str());

        animTable[actionStayingStillS]=new QList<QPixmap*>();
        ss<<"pictures/tux"<<i<<"0_down.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillS], s.c_str());

        animTable[actionStayingStillE]=new QList<QPixmap*>();
        ss<<"pictures/tux"<<i<<"0_right.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillE], s.c_str());

        animTable[actionStayingStillW]=new QList<QPixmap*>();
        ss<<"pictures/tux"<<i<<"0_left.png";
        ss>>s;
        QAnimatedItem::appendNewFrame(animTable[actionStayingStillW], s.c_str());

        animTable[actionBurning]=new QList<QPixmap*>();
        QAnimatedItem::appendNewFrame(animTable[actionBurning], "pictures/tux_burn.png");

    }
}
void QPlayer::walk(const globalDirection dir) {
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
    currentPix=0;
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
    currentPix=0;
}

void QPlayer::burn(){
    currentAnim=burning;
    currentPix=0;

}

void QPlayer::setSick(const bool sick){
    this->sick=sick;
}
