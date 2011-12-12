#ifndef MENUTABFRAME_H
#define MENUTABFRAME_H

#include "ui_MenuTabFrame.h"
#include "ui_MainMenuWidget.h"

class Settings;
class IpStats;
class PlayerListWidget;
class KeySettingsWidget;
class QSound;
class NetClient;
class MapClient;

class MenuTabFrame : public QFrame
{
    Q_OBJECT

public:
    MenuTabFrame(Settings *settings, IpStats *ipStats, PlayerListWidget *playerListWidget, QSound *music, QFrame *parent = 0);
    ~MenuTabFrame();
    QString getAdminPassword() const;
    QGraphicsView *getGraphicPreview() const;
    const QString getPlayerName() const;
    Ui_MainMenuWidget MainUi;
    //Ui_KeySettingsWidget KeysUi;
    Ui_MenuTabFrame TabUi;
    bool setSettings();

private:
    Settings *settings;
    IpStats *ipStats;
    PlayerListWidget *playerListWidget;
    KeySettingsWidget *keySettingsWidget;

    QPixmap loading;

    QSound *music; //TODO, move music handling in StartUi

    void loadNetWidget();
    void loadSound();
    void loadPlayerData();
    void updateNetWidgetState(bool);
    void toggleMusic(bool);//TODO, move music handling in StartUi
    void setAboutBox();

signals:
    void sigMaxPlayersValueChanged(int);
    void sigStart(int);
    void sigDisconnect();
    void sigKickPlayer(qint8);

private slots:
    void isServerChanged(int);
    void soundChanged(int);
    void slotConnectedToServer();
    void slotIsServerAdmin();
    void slotMaxPlayersValueChanged(int);
    void slotMaxWinsValueChanged(int);
    void slotUpdatePlayerData(qint8 playerId, QString playerName);
    void slotMapRandom(bool);
    void slotMapPreviewReceived(MapClient*);
    void slotPlayerLeft(qint8 playerId);
    void slotStartButton();
    void slotDisconnectButton();
    void slotStartServer();
    void slotKickButton();
};


#endif
