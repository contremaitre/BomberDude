#ifndef GAMEFRAME_H
#define GAMEFRAME_H

#include "ui_GameFrame.h"

class IpStats;
class PlayerListWidget;

class GameFrame : public QFrame
{
    Q_OBJECT

public:
    GameFrame(IpStats *ipStats, PlayerListWidget *playerListWidget, QFrame *parent = 0);
    ~GameFrame();
    Ui_GameFrame ui;

private:
    IpStats *ipStats;
    PlayerListWidget *playerListWidget;

signals:
private slots:
};


#endif
