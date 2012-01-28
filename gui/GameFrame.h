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
