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

void IpStats::setPingState(IpState state)
{
    switch(state)
    {
    case STATUS_OK:
        ui.ping_pic->setPixmap(statusGreen);
        break;
    case STATUS_WARN:
        ui.ping_pic->setPixmap(statusYellow);
        break;
    case STATUS_BAD:
        ui.ping_pic->setPixmap(statusRed);
        break;
    }
}

void IpStats::setPacketLostState(IpState state)
{
    switch(state)
    {
    case STATUS_OK:
        ui.pack_loss_pic->setPixmap(statusGreen);
        break;
    case STATUS_WARN:
        ui.pack_loss_pic->setPixmap(statusYellow);
        break;
    case STATUS_BAD:
        ui.pack_loss_pic->setPixmap(statusRed);
        break;
    }
}
