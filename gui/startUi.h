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

#include "Settings.h"
#include "GamePlay.h"
#include "NetClient.h"
#include "ui_main_window.h"


class StartUi : public QMainWindow
{
  Q_OBJECT
public:
    StartUi(QApplication *);
    ~StartUi();

private:
    //QMainWindow mainw;
    Ui_MainWindow *mainWindow;
    GamePlay *gamePlay;
    Settings *settings;
    QSound *music;
    QProcess *server;
    QApplication *qapp;

    QPixmap statusGrey;
    QPixmap statusGreen;
    QPixmap statusYellow;
    QPixmap statusRed;
    QPixmap loading;

    QList <QLabel *> labelsPlayerList;

    void loadPixmaps();
    void loadNetWidget();
    void loadSound();
    void loadIpStats();
    void loadPlayerData();
    bool setSettings();
    void updateNetWidgetState(bool);
    void toggleMusic(bool);

private slots:
    void startServer();
    void isServerChanged(int);
    void maxPlayersValueChanged(int);
    void soundChanged(int);
    void statPing(int);
    void statPacketLoss(double);
    void statsCheckedChanged(int);
    void randomMapCheckedChanged(int);
    void slotConnectionError();
    void slotConnectedToServer();
    void slotServerLaunched();
    void slotServerLaunchedError(QProcess::ProcessError error);
    void slotReadServerDebug();
    void slotIsServerAdmin();
    void slotMaxPlayersChanged(int);
    void slotUpdatePlayerData(qint32 playerId, QString playerName);
    void slotPlayerLeft(qint32 playerId);
    void slotStartGame();
    void slotMapRandom();
    void slotMapLeftButton();
    void slotMapRightButton();
    void slotMapPreviewReceived(MapClient*);
    void slotGameStarted();
    void slotNewPlayerGraphic(int, const QPixmap &);
    void slotServerStopped();
    void closeGame();
};


#endif // QTB_STARTUI_H
