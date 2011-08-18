#ifndef QBOMB_H
#define QBOMB_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class QBomb : public QGraphicsItem
{
private:
    int size,currentPix;
    QList<QPixmap*>* currentAnim;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void initPixList();

public:
    QBomb();
    QBomb(int,int,int);
    static QList<QPixmap*> normalPix;
    static QList<QPixmap*> rcPix;
    static QList<QPixmap*> dudPix;
    void setPos(int,int,int);
    void setRC();
    void setNormalBomb();
    void setDudBomb();
    void nextFrame();
};

#endif // QBOMB_H
