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
