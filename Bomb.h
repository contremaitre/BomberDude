#ifndef BOMB_H
#define BOMB_H

#include <QObject>
#include <QTimer>

class Bomb : public QObject
{
    Q_OBJECT

private:
    int range;
    //owner
    int playerId;
    //duration in ms
    int duration;
    QTimer *blinkTimer;

public:
    Bomb(int range, int playerId, int duration);
    ~Bomb();

signals:
     void aspectChanged();
     void explode();
};

#endif

