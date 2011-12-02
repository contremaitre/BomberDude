#ifndef GAMEFRAME_H
#define GAMEFRAME_H

#include "ui_GameFrame.h"

class IpStats;

class GameFrame : public QFrame
{
    Q_OBJECT

public:
    GameFrame(IpStats *ipStats, QFrame *parent = 0);
    ~GameFrame();
    Ui_GameFrame ui;

private:
    IpStats *ipStats;

signals:
private slots:
};


#endif
