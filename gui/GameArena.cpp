/*
    Copyright (C) 2010,2011 Sébastien Escudier

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

#include <QDebug>
#include <QDataStream>

#include "startUi.h"
#include "Flame.h"
#include "GameArena.h"
#include "Bonus.h"

GameArena::GameArena(int s) :
	map(NULL),
	textHurryUp1(NULL),
	textHurryUp2(NULL),
    timeInSeconds(-999)
{
	width = height = 0;
	maxNbPlayers = 0;
	squareSize = s;
	squaresItem = NULL;
	playersItem = NULL;
    scene = new QGraphicsScene;
    pixmaps.init(squareSize, squareSize);
}

GameArena::~GameArena()
{
    clear();
    delete scene;
}

void GameArena::setGraphicView(QGraphicsView *gv)
{
    graphicView = gv;
    Q_ASSERT(graphicView);
    graphicView->setFocus();
    graphicView->setScene(scene);
}

void GameArena::createGraphics()
{


    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            QGraphicsSquareItem *m_case = getCase(i,j);
            scene->addItem(m_case->getItem());
        }
    }
    /*
    if(!graphicView)
    {
        graphicView = new QGraphicsView(mainWindow);
        int size = squareSize * (width+1);
        mainWindow->setMinimumSize(size,size);
        graphicView->setMinimumSize(size,size);
        graphicView->setScene(scene);
        graphicView->show();
        graphicView->setFocus();
    }
    else
    {
        graphicView->setScene(scene);
    }
    */
}

void GameArena::getEventFilter(QObject *obj)
{
    scene->installEventFilter(obj);
}

void GameArena::clear()
{
    /* if we dont delete squaresItem manually, the scene->clear crash... why ? */
    if (squaresItem)
    {
        for (int i = 0; i < width * height; i++)
            delete squaresItem[i];
        delete[] squaresItem;
        squaresItem = NULL;
    }
    delete []playersItem;
    playersItem = NULL;

    /* remove bombs */
    QMap<int, QGraphicsSquareItem*>::iterator it;
    while( (it = bombs.begin()) != bombs.end())
    {
        delete it.value();
        bombs.erase(it);
    }
    /* remove flames */
    while( (it = flames.begin()) != flames.end())
    {
        delete it.value();
        flames.erase(it);
    }
    /* remove bonus */
    QMap<QPoint, QGraphicsItem*>::iterator itbonus;
    while( (itbonus = bonus.begin()) != bonus.end())
    {
        delete itbonus.value();
        bonus.erase(itbonus);
    }

    /* remove just dead players*/
    while(!burntPlayers.empty())
    {
        QGraphicsSquareItem* item = burntPlayers.takeFirst();
        delete item;
    }

    /* remove options */
    while(!optionsItems.empty())
    {
        QGraphicsSquareItem* item = optionsItems.takeFirst();
        delete item;
    }
    slotRemoveHurryUp();

    scene->clear();
}

void GameArena::setMap(MapClient *newMap)
{

    clear();
    map = newMap;

    width = map->getWidth();
    height = map->getHeight();
    maxNbPlayers = map->getMaxNbPlayers();
    squaresItem = new QGraphicsSquareItem*[width * height];
    playersItem = new playersItem_t[maxNbPlayers];
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            initCase(i,j);
            getCase(i,j)->setItem(pixmaps.getPixmap(map->getType(i,j)));
            if(map->getOption(i,j) != BlockMapProperty::none)
            {
                //add this option on the map. It won't be removed or changed during the game.
                QGraphicsSquareItem* optItem = new QGraphicsSquareItem(i * squareSize,j * squareSize,squareSize);
                optItem->setItem(pixmaps.getPixmap(map->getOption(i,j), map->getOptionDirection(i,j)));
                scene->addItem(optItem);
                optionsItems << optItem;
            }
        }
    }
    qint16 x,y;
    for(int i = 0; i < map->getNbPlayers(); i++)
    {
        map->getPlayerPosition(i,x,y);
        playersItem[i].itemSick = false;
        playersItem[i].sick = false;
        playersItem[i].item.setPos(x-squareSize/2,y-squareSize/2,squareSize);
        playersItem[i].item.setItem(pixmaps.getPixmap(i));
        emit sigNewPlayerGraphic(i,pixmaps.getPixmap(i));
    }

    connect(map, SIGNAL(sigBlockChanged(int)), this, SLOT(blockChanged(int)));
    connect(map, SIGNAL(sigBlockChanged(int,int)), this, SLOT(blockChanged(int,int)));
    connect(map, SIGNAL(sigHeartbeatUpdated(qint32)), this, SLOT(slotHearbeatUpdated(qint32)));

    connect(map, SIGNAL(sigMovePlayer(int, int, int)), this, SLOT(movePlayer(int, int, int)));
    connect(map, SIGNAL(sigPlayerSickChanged(int, bool)), this, SLOT(slotPlayerSickChanged(int, bool)));
    connect(map, SIGNAL(sigKillPlayer(int)), this, SLOT(killPlayer(int)));
    connect(map, SIGNAL(sigAddBonus(Bonus::Bonus_t,qint16,qint16)), this, SLOT(slotAddBonus(Bonus::Bonus_t,qint16,qint16)));
    connect(map, SIGNAL(sigRemoveBonus(qint16,qint16)), this, SLOT(slotRemoveBonus(qint16,qint16)));
    connect(map, SIGNAL(sigAddBomb(int)), this, SLOT(slotAddBomb(int)), Qt::DirectConnection);
    connect(map, SIGNAL(sigMovedBomb(int)), this, SLOT(slotMovedBomb(int)), Qt::DirectConnection);
    connect(map, SIGNAL(sigFlyingBomb(int)), this, SLOT(slotFlyingBomb(int)), Qt::DirectConnection);
    connect(map, SIGNAL(sigRemoveBomb(int)), this, SLOT(slotRemoveBomb(int)), Qt::DirectConnection);
    connect(map, SIGNAL(sigRemoveBombRC(int)), this, SLOT(slotRemoveBombRC(int)), Qt::DirectConnection);
    connect(map, SIGNAL(sigAddFlame(int,qint16,qint16)), this, SLOT(slotAddFlame(int,qint16,qint16)));
    connect(map, SIGNAL(sigRemoveFlame(int)), this, SLOT(slotRemoveFlame(int)));
    createGraphics();
}

void GameArena::movePlayer(int player, int x, int y)
{
    Q_ASSERT(player < map->getNbPlayers()); //Player really exists

    if(playersItem[player].item.getItem()->scene() == 0)
        scene->addItem(playersItem[player].item.getItem());

    playersItem[player].item.setPos(x-squareSize/2,y-squareSize/2,squareSize);
    if(playersItem[player].sick)
    {
        //switch sickness graphic
        if(playersItem[player].itemSick)
            playersItem[player].item.setItem(pixmaps.getPixmap(player));
        else
            playersItem[player].item.setItem(pixmaps.getPixmapSick());
        playersItem[player].itemSick = !playersItem[player].itemSick;
    }
}

void GameArena::slotPlayerSickChanged(int player, bool sick)
{
    Q_ASSERT(player < map->getNbPlayers()); //Player really exists
    playersItem[player].sick = sick;
    //qDebug() <<"slotPlayerSickChanged" ;
    if(!sick && playersItem[player].itemSick)
    {
        playersItem[player].item.setItem(pixmaps.getPixmap(player));//remove sickness graphic
        playersItem[player].itemSick = false;
    }
}

int GameArena::getCaseSize() const
{
	return squareSize;
}

void GameArena::initCase(int i, int j)
{
	int x_a = i*squareSize;
	int y_a = j*squareSize;
	squaresItem[j*width+i] = new QGraphicsSquareItem(x_a,y_a,squareSize);
}

void GameArena::blockChanged(int pos)
{
	QGraphicsSquareItem* tempItem = getCase(pos);
	//qDebug() << "Wall: " << tempItem->getItem()->pixmap().cacheKey() << ", new value: " << pixmaps.getPixmap(map->getType(pos)).cacheKey();
	tempItem->setItem(pixmaps.getPixmap(map->getType(pos)));
}

void GameArena::blockChanged(int i, int j)
{
	blockChanged(j * width + i);
}

const MapClient *GameArena::getMap()
{
	return map;
}

int GameArena::getWidth()
{
	return width;
}

int GameArena::getHeight()
{
	return height;
}

int GameArena::getNbPlayers() const
{
	return maxNbPlayers;
}

QGraphicsSquareItem *GameArena::getCase(int i, int j)
{
	return squaresItem[j*width+i];
}

QGraphicsSquareItem *GameArena::getCase(int pos)
{
	return squaresItem[pos];
}

void GameArena::slotHearbeatUpdated(qint32 value) {
    int newTime = value / (1000 / HEARTBEAT);

    // rounding for negative values offset by 1
    if(value < 0)
        --newTime;

    if(newTime != timeInSeconds) {
        timeInSeconds = newTime;
        emit sigTimeUpdated(newTime);
    }

    if(value == 0)
    {
        QFont bigText;
        bigText.setPointSize(32);
        bigText.setWeight(QFont::Bold);

        textHurryUp1 = new QGraphicsSimpleTextItem();
        textHurryUp1->setText("HURRY UP !");
        textHurryUp1->setFont(bigText);
        textHurryUp1->setZValue(1.0);
        scene->addItem(textHurryUp1);
        textHurryUp2 = new QGraphicsSimpleTextItem();
        textHurryUp2->setText(textHurryUp1->text());
        textHurryUp2->setFont(bigText);
        textHurryUp2->setZValue(0.9);
        textHurryUp2->setX(textHurryUp2->x() + 1);
        textHurryUp2->setY(textHurryUp2->y() + 1);
        textHurryUp2->setBrush(QBrush(Qt::white));
        scene->addItem(textHurryUp2);
        QTimer::singleShot(3000, this, SLOT(slotRemoveHurryUp()));
    }
}

void GameArena::removeBurnt() {
    QGraphicsSquareItem* item = burntPlayers.first();
    burntPlayers.pop_front();
    delete item;
}

void GameArena::slotRemoveHurryUp()
{
    if(textHurryUp1 && textHurryUp2)
    {
        delete textHurryUp1;
        delete textHurryUp2;
        textHurryUp1 = NULL;
        textHurryUp2 = NULL;
    }
}

void GameArena::killPlayer(int id)
{
	qint16 px, py;
	map->getPlayerPosition(id, px, py);
	scene->removeItem(playersItem[id].item.getItem());
	QGraphicsSquareItem* burnt = new QGraphicsSquareItem(px-squareSize/2, py-squareSize/2, squareSize);
	burnt->setItem(pixmaps.getPixmapBurnt());
	scene->addItem(burnt);
	burntPlayers.append(burnt);
	QTimer::singleShot(1500, this, SLOT(removeBurnt()));
}

void GameArena::slotAddBonus(Bonus::Bonus_t bonusType, qint16 x, qint16 y) {
    QGraphicsSquareItem* pixBonus = new QGraphicsSquareItem(x * squareSize,
                                                            y * squareSize,
                                                            squareSize);
    pixBonus->setItem(pixmaps.getPixmap(bonusType));

    QPoint key(x, y);
    QMap<QPoint, QGraphicsItem*>::iterator itb = bonus.find(key);
    if(itb != bonus.end())
        delete itb.value();
    bonus[key] = pixBonus;
    scene->addItem(pixBonus);    
}

void GameArena::slotRemoveBonus(qint16 x, qint16 y) {
    QPoint key(x, y);
    QMap<QPoint, QGraphicsItem*>::iterator itb = bonus.find(key);
    if(itb != bonus.end()) {
        delete itb.value();
        bonus.erase(itb);
    }
}

void GameArena::slotAddBomb(int id)
{
    const BombClient& bomb = map->getRefBomb(id);
    QGraphicsSquareItem* pix = new QGraphicsSquareItem(bomb.getX() - squareSize/2,
                                                       bomb.getY() - squareSize/2,
                                                       squareSize);
    pix->setZValue(1.0);
    if(bomb.getIsRC())
        pix->setItem(pixmaps.getPixmapBombrc());
    else if(bomb.getDudBomb())
        pix->setItem(pixmaps.getPixmapBombDud());
    else
        pix->setItem(pixmaps.getPixmapBomb());
    bombs[id] = pix;
    scene->addItem(pix->getItem());
}

void GameArena::slotMovedBomb(int id)
{
    const BombClient& bomb = map->getRefBomb(id);
    bombs[id]->setPos(bomb.getX() - squareSize/2, bomb.getY() - squareSize/2, squareSize);
}

void GameArena::slotFlyingBomb(int id)
{
    const BombClient& bomb = map->getRefBomb(id);
    //qDebug() << "GameArena : new flying bomb";
    /* Todo : animate the flying bomb */
}

void GameArena::slotRemoveBomb(int id)
{
    QMap<int, QGraphicsSquareItem*>::iterator itb = bombs.find(id);
    if(itb != bombs.end()) {
        delete itb.value();
        bombs.erase(itb);
    }
}

void GameArena::slotRemoveBombRC(int id)
{
    QMap<int, QGraphicsSquareItem*>::iterator itb = bombs.find(id);
    if(itb != bombs.end()) {
        //Todo : we could change the pixmap instead of creating a new one
        delete  itb.value();
        const BombClient& bomb = map->getRefBomb(id);
        QGraphicsSquareItem* pix = new QGraphicsSquareItem(bomb.getX() - squareSize/2,
                                                           bomb.getY() - squareSize/2,
                                                           squareSize);
        pix->setItem(pixmaps.getPixmapBomb());
        *itb = pix;
        scene->addItem(pix);
        qDebug() << "gamearene : bomb lost rc";
    }
}

void GameArena::slotMapWinner(qint8 playerId) {
    QFont bigText;
    bigText.setPointSize(32);
    bigText.setWeight(QFont::Bold);

    QGraphicsSimpleTextItem* textOver = new QGraphicsSimpleTextItem();
    if(playerId == -1)
        textOver->setText("DRAW GAME!");
    else
        textOver->setText(QString("PLAYER ") + QString::number(playerId + 1) + QString(" WINS!"));
    textOver->setFont(bigText);
    textOver->setZValue(1.0);

    scene->addItem(textOver);

    QGraphicsSimpleTextItem* text2 = new QGraphicsSimpleTextItem();
    text2->setText(textOver->text());
    text2->setFont(bigText);
    text2->setZValue(0.9);
    text2->setX(text2->x() + 1);
    text2->setY(text2->y() + 1);
    text2->setBrush(QBrush(Qt::white));

    scene->addItem(text2);
}

void GameArena::slotAddFlame(int id, qint16 x, qint16 y)
{
    QGraphicsSquareItem* pixBonus = new QGraphicsSquareItem(x * squareSize,
                                                            y * squareSize,
                                                            squareSize);
    pixBonus->setItem(pixmaps.getPixmapFlame());

    flames.insertMulti(id,pixBonus);
    scene->addItem(pixBonus);
}

void GameArena::slotRemoveFlame(int id)
{
    QMap<int, QGraphicsSquareItem*>::iterator itb = flames.find(id);
    while(itb != flames.end() && itb.key() == id) {
        delete itb.value();
        ++itb;
    }
    flames.remove(id);
}
