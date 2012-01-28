/*
    Copyright (C) 2012 Sébastien Escudier

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

#include "InterGameFrame.h"
#include "PlayerListWidget.h"

InterGameFrame::InterGameFrame(PlayerListWidget *playerListWidget, bool admin, QFrame *parent)
                        : QFrame(parent), playerListWidget(playerListWidget)
{
    ui.setupUi(this);
    playerListWidget->setParent(ui.playerListWidgetHolder);
    ui.nextRoundButton->setEnabled(admin);
}

InterGameFrame::~InterGameFrame()
{
    playerListWidget->setParent(NULL);
}
