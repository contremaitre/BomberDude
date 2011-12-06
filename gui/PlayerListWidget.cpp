#include <QDebug>
#include <QLabel>
#include "PlayerListWidget.h"

PlayerListWidget::PlayerListWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
}

void PlayerListWidget::slotAddPlayer(qint32 playerId, QString name)
{
    qDebug() << "PlayerListWidget new player" << playerId << name;
    QLayoutItem * old = ui.playerListLayout->itemAtPosition ( playerId, 1 );
    if(old)
    {
        dynamic_cast<QLabel*>(old->widget())->setText(name);
    }
    else
    {
        QLabel *label = new QLabel(name);
        ui.playerListLayout->addWidget(label,playerId,1);
    }
}

void PlayerListWidget::slotNewPlayerGraphic(int playerId, const QPixmap &pix)
{
    qDebug() << "PlayerListWidget new graphic" << playerId;
    QLayoutItem * old = ui.playerListLayout->itemAtPosition ( playerId, 0 );
    if(old)
    {
        dynamic_cast<QLabel*>(old->widget())->setPixmap(pix);
    }
    else
    {
        QLabel *label = new QLabel();
        label->setPixmap(pix);
        ui.playerListLayout->addWidget(label,playerId,0);
    }
}

void PlayerListWidget::slotUpdatePlayerScore(int playerId, int score)
{
    qDebug() << "PlayerListWidget new score" << playerId;
    QLayoutItem * old = ui.playerListLayout->itemAtPosition ( playerId, 2 );
    if(old)
    {
        dynamic_cast<QLabel*>(old->widget())->setText(QString::number(score));
    }
    else
    {
        QLabel *label = new QLabel(QString::number(score));
        ui.playerListLayout->addWidget(label,playerId,2);
    }
}

void PlayerListWidget::slotRemovePlayer(qint32 playerId)
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

PlayerListWidget::~PlayerListWidget()
{
    QLayoutItem *child;
    while ((child = ui.playerListLayout->takeAt(0)) != 0)
    {
        delete child->widget();
        delete child;
    }
}
