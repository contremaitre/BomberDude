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
 * This is where the Network server, maps and graphic can interact
 * Player input keys from the scene are captured here
 */


#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <QObject>

class GameField;
class NetClient;
class NetServer;
class Map;
class QMainWindow;
class Settings;
class QTimer;
class QSound;


class GamePlay : public QObject
{
  Q_OBJECT

public:
    GamePlay(QMainWindow *, Settings *);
    ~GamePlay();
    void launch();

private:
    GameField *gameField;
    NetClient *client;
    NetServer *server; //instanciated if we are the server
    Settings *settings;

    bool leftK, rightK, upK, downK;
    QTimer *timer;
    QTimer *timerPing;
    QSound *music;
    
    //inherited from QObject and redefined
    bool eventFilter(QObject *obj, QEvent *event);

    /**
     * Request a move
     * \param direction
     */
    void move(int);

private slots:
    void slotStart();
    void slotMoveTimer();
    void dropBomb();
    void slotPingTimer();
    void moveReceived(qint16 plId, qint16 x, qint16 y);
    void bombReceived(qint16 plId, qint16 x, qint16 y);
    void mapReceived(const Map*);
    void slotClientConnected();
    void slotClientConnectError();
    void slotServerError();
    void slotServerReady();

signals:
    void connectedToServer();
    void connectionError();
    void quitGame();
};

#endif

