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

#ifndef STARTUI_H
#define STARTUI_H
#include <QObject>
#include <QMainWindow>
#include <QLayout>

class Ui_MainWindow;
class GamePlay;
class Settings;
class QSound;

class StartUi : public QMainWindow
{
  Q_OBJECT
public:
    StartUi();
    ~StartUi();

private:
    //QMainWindow mainw;
    Ui_MainWindow *mainWindow;
    GamePlay *gamePlay;
    Settings *settings;
    QSound *music;

    QPixmap statusGrey;
    QPixmap statusGreen;
    QPixmap statusYellow;
    QPixmap statusRed;

    void loadPixmaps();
    void loadNetWidget();
    void loadSound();
    void loadIpStats();
    bool setSettings();
    void setAddrFieldEnabled(bool);
    void toggleMusic(bool);

private slots:
    void start();
    void isServerChanged(int);
    void soundChanged(int);
    void statPing(int);
    void statPacketLoss(double);
    void statsCheckedChanged(int);
    void slotConnectionError();
    void slotConnected();
    void closeGame();
};

#endif

