#ifndef IPSTATS_H
#define IPSTATS_H

#include "ui_IpStats.h"


class IpStats : public QWidget
{
    Q_OBJECT

public:
    IpStats(QWidget *parent = 0);

private:
    Ui_IpStats ui;
    QPixmap statusGrey;
    QPixmap statusGreen;
    QPixmap statusYellow;
    QPixmap statusRed;

    void loadPixmaps();

private slots:
    void slotStatPacketLoss(double);
    void slotStatPing(int);
};


#endif
