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

#include "GameFrame.h"
#include "IpStats.h"
#include "PlayerListWidget.h"

GameFrame::GameFrame(IpStats *ipStats, PlayerListWidget *playerListWidget, QFrame *parent)
                        : QFrame(parent), ipStats(ipStats), playerListWidget(playerListWidget)
{
    ui.setupUi(this);
    ipStats->setParent(ui.IpStatHolder);
    playerListWidget->setParent(ui.playerListWidgetHolder);
}

GameFrame::~GameFrame()
{
    ipStats->setParent(NULL);
    playerListWidget->setParent(NULL);
}
