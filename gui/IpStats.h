#ifndef IPSTATS_H
#define IPSTATS_H

#include "ui_IpStats.h"

enum IpState { STATUS_OK,
                STATUS_WARN,
                STATUS_BAD,
              };

class IpStats : public QWidget
{
    Q_OBJECT

public:
    IpStats(QWidget *parent = 0);
    void setPingState(IpState state);
    void setPacketLostState(IpState state);
private:
    Ui_IpStats ui;
    QPixmap statusGrey;
    QPixmap statusGreen;
    QPixmap statusYellow;
    QPixmap statusRed;

    void loadPixmaps();

signals:

private slots:
};


#endif
