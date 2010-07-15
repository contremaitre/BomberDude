#include "plateau.h"

plateau::plateau()
{
    scene = new QGraphicsScene;
    scene->addItem(caseItem.rectItem);
    view = new QGraphicsView(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->show();
}
