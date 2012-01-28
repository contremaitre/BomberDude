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

#include <QDebug>
#include <QLabel>
#include "PlayerListWidget.h"

PlayerListWidget::PlayerListWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
}

void PlayerListWidget::slotAddPlayer(qint8 playerId, QString name)
{
    qDebug() << "PlayerListWidget new player" << playerId << name;
    QLayoutItem * old = ui.playerListLayout->itemAtPosition ( playerId, 1 );
    if(old && old->widget())
    {
        QLabel* oldLabel = dynamic_cast<QLabel*>(old->widget());
        if(!oldLabel)
        {
            delete old->widget();
            ui.playerListLayout->removeItem(old);
            old = NULL;
        }
        else
            oldLabel->setText(name);
    }
    if(!old || !old->widget())
    {
        QLabel *label = new QLabel(name);
        ui.playerListLayout->addWidget(label,playerId,1);
        slotUpdatePlayerScore(playerId, 0);
    }
}

void PlayerListWidget::slotNewPlayerGraphic(qint8 playerId, const QPixmap &pix)
{
    qDebug() << "PlayerListWidget new graphic" << playerId;
    QLayoutItem * old = ui.playerListLayout->itemAtPosition ( playerId, 0 );
    if(old)
    {
        QLabel* oldLabel = dynamic_cast<QLabel*>(old->widget());
        if(!oldLabel)
        {
            delete old->widget();
            ui.playerListLayout->removeItem(old);
            old = NULL;
        }
        else
            oldLabel->setPixmap(pix);
    }
    if(!old || !old->widget())
    {
        QLabel *label = new QLabel();
        label->setPixmap(pix);
        ui.playerListLayout->addWidget(label,playerId,0);
    }
}

void PlayerListWidget::slotUpdatePlayerScore(qint8 playerId, qint16 score)
{
    qDebug() << "PlayerListWidget new score" << playerId << score;
    QLayoutItem * old = ui.playerListLayout->itemAtPosition ( playerId, 2 );
    if(old)
    {
        QLabel* oldLabel = dynamic_cast<QLabel*>(old->widget());
        if(!oldLabel)
        {
            delete old->widget();
            ui.playerListLayout->removeItem(old);
            old = NULL;
        }
        else
            oldLabel->setText(QString::number(score));
    }
    if(!old || !old->widget())
    {
        QLabel *label = new QLabel(QString::number(score));
        ui.playerListLayout->addWidget(label,playerId,2);
    }
}

void PlayerListWidget::slotRemovePlayer(qint8 playerId)
{
    QLayoutItem *child;
    for(int i = 0; i < 3; i++)
    {
        if( (child = ui.playerListLayout->itemAtPosition ( playerId, i )) )
        {
            delete child->widget();
            ui.playerListLayout->removeItem(child);
        }
    }
}

void PlayerListWidget::clear()
{
    QLayoutItem *child;
    while ((child = ui.playerListLayout->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }
}

PlayerListWidget::~PlayerListWidget()
{
    clear();
}
