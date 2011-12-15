#include <QtGui>

#include "IpStats.h"

IpStats::IpStats(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    loadPixmaps();
    ui.ping_pic->setPixmap(statusGrey);
    ui.pack_loss_pic->setPixmap(statusGrey);

}

void IpStats::loadPixmaps()
{
    QSize size(ui.ping_pic->width(),ui.ping_pic->height());
    QPixmap grey = QPixmap("pictures/status_grey.png");
    statusGrey = grey.scaled(size);

    QPixmap yellow = QPixmap("pictures/status_yellow.png");
    statusYellow = yellow.scaled(size);

    QPixmap green = QPixmap("pictures/status_green.png");
    statusGreen = green.scaled(size);

    QPixmap red = QPixmap("pictures/status_red.png");
    statusRed = red.scaled(size);

}

void IpStats::slotStatPacketLoss(double packet_loss)
{
    if (packet_loss == 0)
        ui.pack_loss_pic->setPixmap(statusGreen);
    else if (packet_loss <= 0.01)
        ui.pack_loss_pic->setPixmap(statusYellow);
    else
        ui.pack_loss_pic->setPixmap(statusRed);
}

void IpStats::slotStatPing(int ping)
{
    if (ping < 60)
        ui.ping_pic->setPixmap(statusGreen);
    else if (ping < 100)
        ui.ping_pic->setPixmap(statusYellow);
    else
        ui.ping_pic->setPixmap(statusRed);
}
