/*
    Copyright (C) 2010 Sébastien Escudier

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

/**
 *
 * This is the main graphic class
 * It will draw objects and animations
 *
 */

#ifndef QTB_STARTUI_H
#define QTB_STARTUI_H


#include <QObject>
#include <QMainWindow>
#include <QLayout>
#include <QProcess>
#include <QSound>

#include "NetClient.h"
#include "ui_MainWindow.h"
#include "MenuTabFrame.h"
#include "IpStats.h"
#include "PlayerListWidget.h"

class Settings;
class GameFrame;
class MenuTabFrame;
class GamePlay;
class InterGameFrame;

class StartUi : public QMainWindow, private Ui_MainWindow
{
  Q_OBJECT
public:
    StartUi(QApplication *);
    ~StartUi();

private:

    MenuTabFrame *menuTabFrame;
    GamePlay *gamePlay;
    GameFrame *gameFrame;
    InterGameFrame *interGameFrame;
    PlayerListWidget playerListWidget;
    IpStats ipStats;
    Settings *settings;
    QSound *music;
    QProcess *server;
    QApplication *qapp;
    QString adminPassword;

    int styleIndex; //remember the map style the first time we start the game

    void loadMenuTabFrame();
    void loadGameFrame();
    void loagInterGameFrame();

private slots:
    void slotStartServer();
    void slotMaxPlayersValueChanged(int);
    void slotMaxWinsValueChanged(int);
    void randomMapCheckedChanged(int);
    void slotConnectionError();
    void slotServerLaunched();
    void slotServerLaunchedError(QProcess::ProcessError error);
    void slotReadServerDebug();
    void slotStartGame(int);
    void slotNextRound();
    void slotDisconnectGame();
    void slotMapLeftButton();
    void slotMapRightButton();
    void slotGameStarted();
    void slotServerStopped();
    void closeGame();
    void slotKickPlayer(qint8);
    void slotEndRound(qint8, bool);
    void slotLoadInterGame();
    void slotAddLocalPlayer();
};


#endif // QTB_STARTUI_H
