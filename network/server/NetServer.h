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
 * This class communicates with every client connected to the game
 * 
 * When a move is requested we check if it is valid on the map, we apply it,
 * and we send that move to every client
 *
 */

#ifndef QTB_NETSERVER_H
#define QTB_NETSERVER_H


#include <QThread>
#include <QHostAddress>
#include <QTcpServer>
#include <QUdpSocket>
#include <QStringList>
#include <QFileInfo>

#include "Flame.h"
#include "NetServerClient.h"

class QDir;
class MapServer;

class NetServer : public QThread
{
    Q_OBJECT
public:
    NetServer(int port, QString adminPasswd, bool debugMode, bool startedFromGui);
    ~NetServer();
    void run();
    void close();
    void restart(); //ends the game, remove all players and wait for new players
    //call this function when the game is launched
    bool loadMap(int);
    void setMaxPlayers(int);
    void startGame(int);
    void selectMap(qint8 direction);
    bool passwordReceived(int id, QString &pass);
    void shutdown();

private:
    MapServer *map;
    MapServer *mapPreview;
    QFileInfoList mapList;
    bool randomMap;
    int currentMapInList;
    int port;
    bool gameStarted;
    int maxNbPlayers;
    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;
    /* Connected client list, sorted by id */
    QList<NetServerClient *> clients;
    QString adminPasswd; //optional admin password
    bool adminConnected;
    int mapW, mapH, blockSize; //map size if it's a random map
    bool debugMode;
    bool startedFromGui;

    int readMove(QDataStream &in);
    void sendUdpWelcomeAck(QHostAddress sender, quint16 senderPort);
    void sendPingBack(quint32 cpt, QHostAddress sender, quint16 senderPort);
    void sendCLientDisconnected(int playerId);
    void allocMap();
    void setBlockSize(const QByteArray &block, QDataStream & out);

private slots:
    void incomingClient();
    void clientDisconected(NetServerClient *);
    void receiveUdp();
    void slotUpdatePlayerData(int playerId, QString playerName);

	// to be called in its own thread (otherwise the QTimer object can fail to send signals)
	void startHeartBeat();

	// sent by the game loop
	void updateMap(QByteArray updateData);
    void slotWinner(qint8 playerId);

signals:
    void newPlayer();
    void allPlayersLeft();
    void serverReady();
    void sigStartHeartBeat();
    void sigQuit();
    void sigAdminGuiDisconnected();
};


#endif // QTB_NETSERVER_H
