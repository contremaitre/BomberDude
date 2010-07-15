#include "Board.h"

Board::Board(int nb_x, int nb_y)
{
    scene = new QGraphicsScene;
    caseList.init(nb_x,nb_y);
    for(int i = 0; i < nb_x; i++)
    {
        for(int j = 0; j < nb_y; j++)
        {
            QGraphicsCaseItem *m_case = caseList.getCase(i,j);
            scene->addItem(m_case->rectItem);
        }
    }
    view = new QGraphicsView(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->show();
}

