#ifndef QFLAME_H
#define QFLAME_H

#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class QFlame : public QGraphicsItem
{
private:
    int size,currentPix;
    QList<QPixmap*>* currentAnim;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void initPixList();

public:
    QFlame();
    QFlame(int,int,int);
    static QList<QPixmap*> explosionPix;
    void setPos(int,int,int);
    void nextFrame();
};

#endif // QFLAME_H
