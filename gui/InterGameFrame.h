#ifndef INTERGAMEFRAME_H
#define INTERGAMEFRAME_H

#include "ui_InterGameFrame.h"

class PlayerListWidget;

class InterGameFrame : public QFrame
{
    Q_OBJECT

public:
    InterGameFrame(PlayerListWidget *playerListWidget, bool admin, QFrame *parent = 0);
    ~InterGameFrame();
    Ui_InterGameFrame ui;

private:
    PlayerListWidget *playerListWidget;

};


#endif
