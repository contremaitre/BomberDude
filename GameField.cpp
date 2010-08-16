#include "GameField.h"
#include <QKeyEvent>

GameField::GameField(int nb_x, int nb_y, int s)
{
    scene = new QGraphicsScene;
    caseList.init(nb_x,nb_y,s);
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

bool GameField::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::KeyRelease)
    {
        qDebug("gamefield eventFilter key press event");
        QKeyEvent *c = ( QKeyEvent * )event;
        if(c->key() == Qt::Key_Escape)
        {
            qDebug("escape");
        }
        else if(c->key() == Qt::Key_Left)
        {
            qDebug("left");
        }
        else if(c->key() == Qt::Key_Up)
        {
            qDebug("up");
        }
        else if(c->key() == Qt::Key_Down)
        {
            qDebug("down");
        }
        else if(c->key() == Qt::Key_Right)
        {
            qDebug("right");
        }
    }
    return false;
}

