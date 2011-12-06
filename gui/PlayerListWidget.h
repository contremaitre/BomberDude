#ifndef PLAYERLISTFRAME_H
#define PLAYERLISTFRAME_H

#include "ui_PlayerListWidget.h"

class QLabel;

class PlayerListWidget : public QWidget
{
    Q_OBJECT

public:
    PlayerListWidget(QWidget *parent = 0);
    ~PlayerListWidget();
    Ui_PlayerListWidget ui;
/*
    removePlayer(int playerId);
    setScore(int playerId, int score);*/

private slots:
    void slotAddPlayer(qint32 playerId, QString name);
    void slotNewPlayerGraphic(int playerId, const QPixmap &pix);
    void slotUpdatePlayerScore(int playerId, int score);
    void slotRemovePlayer(qint32 playerId);
};


#endif
