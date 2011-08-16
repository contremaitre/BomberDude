#ifndef QBOMB_H
#define QBOMB_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class QBomb : public QGraphicsItem
{
private:
    int x,y,size,currentPix;
    QRectF bRect;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QList<QPixmap*>  pixmaps;

    void initPixList();

public:
    QBomb();
    QBomb(int,int,int);
    void setPos(int,int,int);
    void nextFrame();
};

#endif // QBOMB_H
