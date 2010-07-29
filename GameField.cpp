#include "GameField.h"

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
}

GameField::~GameField()
{
    delete view;
}

