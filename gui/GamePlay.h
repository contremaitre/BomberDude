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


#ifndef QTB_GAMEPLAY_H
#define QTB_GAMEPLAY_H


#include <QObject>
#include <QTimer>
#include <QMainWindow>

#include "GameArena.h"
#include "NetClient.h"
//#include "Map.h"
#include "Settings.h"


class GamePlay : public QObject
{
  Q_OBJECT

public:
    GamePlay(Settings *, QGraphicsView *, QString);
    ~GamePlay();
    /* control keys are set in this function */
    void cliConnect(const QString &pass);
    NetClient *getNetClient();
    void gameStarted(QGraphicsView *);
    void addPlayer(const QString &);

private:
    GameArena *gameArena;
    GameArena *gameArenaPreview; //todo : create a light class for the preview
    QGraphicsView *mapGraphicPreview;
    NetClient *client;
    NetClient *clientPlayer2;
    QString player1name;
    QString player2name;

    Settings *settings;

    /* remember which key is pressed */
    bool leftK1, rightK1, upK1, downK1;
    bool leftK2, rightK2, upK2, downK2;
    key_set_t player1Keys;
    key_set_t player2Keys;
    QTimer timer;
    QTimer timerPing;
    
    //inherited from QObject and redefined
    bool eventFilter(QObject *obj, QEvent *event);

    /**
     * Request a move
     * \param direction
     */
    void move(int);

private slots:
    void slotMoveTimer();
    void slotPingTimer();
    void mapReceived(MapClient*);
//	void updateMap(QByteArray updateBlock);
    void mapPreviewReceived(MapClient*);
    void slotMapRandom(bool);
    void slotTimeUpdated(int timeInSeconds);
    void slotNewPlayerGraphic(qint8,const QPixmap &);
    void slotConnectedToServer();
    void slotPlayer2Disconnected();
    void slotEndRound(qint8, bool end);
    void slotLoadInterGame();

signals:
    void connectionError();
    void quitGame();
    void sigStatPing(int);
    void sigStatPacketLoss(double);
    void sigIsServerAdmin(int);
    void sigTimeUpdated(int timeInSeconds);
    void sigNewPlayerGraphic(qint8, const QPixmap &);
    void sigLoadInterGame();

};


#endif // QTB_GAMEPLAY_H
