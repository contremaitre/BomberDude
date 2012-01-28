/*
    Copyright (C) 2011 Sébastien Escudier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
