#include "GameField.h"
#include <QKeyEvent>

GameField::GameField(int nb_x, int nb_y, int s)
{
    scene = new QGraphicsScene;
    caseList.init(nb_x,nb_y,s);
    connect(&caseList, SIGNAL(pixmapChanged(int)),this, SLOT(pixmapChanged(int)));
    for(int i = 0; i < nb_x; i++)
    {
        for(int j = 0; j < nb_y; j++)
        {
            QGraphicsCaseItem *m_case = caseList.getCase(i,j);
            scene->addItem(m_case->getItem());
        }
    }
    view = new QGraphicsView(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->show();
    scene->installEventFilter(this);
}

GameField::~GameField()
{
    delete view;
}

/**
 *      1
 *      |
 *  0 <- -> 2
 *      |
 *      3
 */
bool GameField::move(int id, int direction)
{
    int x,y;
    caseList.getPlayerPosition(id,x,y);
    bool ok = false;
    qDebug("player %d, direction %d, pos (%d,%d)",id,direction,x,y);
    switch(direction)
    {
        case 0:
            ok = caseList.movePlayer(id,x-1,y);
            break;

        case 1:
            ok = caseList.movePlayer(id,x,y-1);        
            break;

        case 2:
            ok = caseList.movePlayer(id,x+1,y);        
            break;
        case 3:
            ok = caseList.movePlayer(id,x,y+1);        
            break;
    }
    qDebug("move %s",ok?"ok":"failed");
    return ok;
}

void GameField::pixmapChanged(int pos)
{
    qDebug("GameField pixmapChanged %d",pos);
    QGraphicsCaseItem *m_case = caseList.getCase(i,j);
    scene->addItem(m_case->getItem());
}

bool GameField::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *c = ( QKeyEvent * )event;
        if(c->key() == Qt::Key_Escape)
        {
            qDebug("escape");
        }
        else if(c->key() == Qt::Key_Left)
        {
            move(0,0);
        }
        else if(c->key() == Qt::Key_Up)
        {
            move(0,1);
        }
        else if(c->key() == Qt::Key_Right)
        {
            move(0,2);
        }
        else if(c->key() == Qt::Key_Down)
        {
            move(0,3);
        }
    }
    return false;
}

