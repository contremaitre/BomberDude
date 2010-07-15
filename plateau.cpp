#include "plateau.h"


plateau::plateau()
{
    scene = new QGraphicsScene;
    scene->addRect(QRectF(-10, -10, 20, 20));
    view = new QGraphicsView(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    view->show();
}
