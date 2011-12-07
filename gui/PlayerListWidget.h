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
    void clear();
    Ui_PlayerListWidget ui;

private slots:
    void slotAddPlayer(qint8 playerId, QString name);
    void slotNewPlayerGraphic(qint8 playerId, const QPixmap &pix);
    void slotUpdatePlayerScore(qint8 playerId, qint16 score);
    void slotRemovePlayer(qint8 playerId);
};


#endif
