#ifndef MENUFRAME_H
#define MENUFRAME_H

#include "ui_MenuFrame.h"

class Settings;
class IpStats;
class QSound;
class NetClient;
class MapClient;

class MenuFrame : public QFrame
{
    Q_OBJECT

public:
    MenuFrame(Settings *settings, IpStats *ipStats, QSound *music, QFrame *parent = 0);
    ~MenuFrame();
    QString getAdminPassword() const;
    QGraphicsView *getGraphicPreview() const;
    const QString getPlayerName() const;
    void setNetClient(NetClient *);
    Ui_MenuFrame ui;
    bool setSettings();

private:
    Settings *settings;
    IpStats *ipStats;
    QPixmap loading;

    QSound *music; //TODO, move music handling in StartUi

    void loadNetWidget();
    void loadSound();
    void loadPlayerData();
    void updateNetWidgetState(bool);
    void toggleMusic(bool);//TODO, move music handling in StartUi

signals:
    void sigMaxPlayersValueChanged(int);
    void sigStart(int);
    void sigDisconnect();

private slots:
    void isServerChanged(int);
    void soundChanged(int);
    void slotConnectedToServer();
    void slotIsServerAdmin();
    void slotMaxPlayersValueChanged(int);
    void slotUpdatePlayerData(qint32 playerId, QString playerName);
    void slotMapRandom(bool);
    void slotMapPreviewReceived(MapClient*);
    void slotPlayerLeft(qint32 playerId);
    void slotStartButton();
    void slotDisconnectButton();
    void slotStartServer();
};


#endif
