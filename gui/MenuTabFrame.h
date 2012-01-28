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
