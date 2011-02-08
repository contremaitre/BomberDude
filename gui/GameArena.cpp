/*
    Copyright (C) 2010 Sébastien Escudier

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

GameArena::GameArena(QMainWindow * mainw, QGraphicsView *view, int s) :
	map(NULL),
    timeInSeconds(-999)
{
	width = height = 0;
	maxNbPlayers = 0;
	squareSize = s;
	squaresItem = NULL;
	playersItem = NULL;
    scene = new QGraphicsScene;
    mainWindow = mainw;
    graphicView = view;
    pixmaps.init(squareSize, squareSize);
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
}

void GameArena::getEventFilter(QObject *obj)
{
    scene->installEventFilter(obj);
}

void GameArena::clear()
{
    if (squaresItem)
    {
        for (int i = 0; i < width * height; i++)
            delete squaresItem[i];
        delete[] squaresItem;
        squaresItem = NULL;
    }
    delete[] playersItem;
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
    connect(map, SIGNAL(sigRemoveBomb(int)), this, SLOT(slotRemoveBomb(int)), Qt::DirectConnection);
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
/*
QGraphicsSquareItem *GameArena::getPlayer(int id)
{
    return &playersItem[id].item;
}
*/
void GameArena::slotHearbeatUpdated(qint32 value) {
    int newTime = value / (1000 / HEARTBEAT);

    // rounding for negative values offset by 1
    if(value < 0)
        --newTime;

    if(newTime != timeInSeconds) {
        timeInSeconds = newTime;
        emit sigTimeUpdated(newTime);
    }
}

void GameArena::removeBurnt() {
    QGraphicsSquareItem* item = burntPlayers.first();
    burntPlayers.pop_front();
    scene->removeItem(item);
    delete item;
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
        scene->removeItem(itb.value());
    bonus[key] = pixBonus;
    scene->addItem(pixBonus);    
}

void GameArena::slotRemoveBonus(qint16 x, qint16 y) {
    QPoint key(x, y);
    QMap<QPoint, QGraphicsItem*>::iterator itb = bonus.find(key);
    if(itb != bonus.end()) {
        scene->removeItem(itb.value());
        bonus.erase(itb);
    }
}

void GameArena::slotAddBomb(int id)
{
    const Bomb *bomb = map->getBomb(id);
    QGraphicsSquareItem* pix = new QGraphicsSquareItem(bomb->x * squareSize,
                                                       bomb->y * squareSize,
                                                            squareSize);
    qDebug() << "new bomb, remote =" << bomb->remoteControled;
    pix->setItem(pixmaps.getPixmap(bomb->remoteControled ? BlockMapProperty::bombrc : BlockMapProperty::bomb));
    bombs[id] = pix;
    scene->addItem(pix);
}

void GameArena::slotRemoveBomb(int id)
{
    QMap<int, QGraphicsItem*>::iterator itb = bombs.find(id);
    if(itb != bombs.end()) {
        scene->removeItem(itb.value());
        bombs.erase(itb);
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

GameArena::~GameArena()
{
    clear();
    delete graphicView;
    //delete scene; todo crash ?
}

