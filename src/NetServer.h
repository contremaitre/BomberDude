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

#ifndef NETSERVER_H
#define NETSERVER_H

#include <QThread>
#include <QHostAddress>

#include "Flame.h"
class MapServer;
class QTcpServer;
class QUdpSocket;
class NetServerClient;

class NetServer : public QThread
{
    Q_OBJECT
public:
    NetServer(const MapServer *, int port);
    NetServer(int port);
    ~NetServer();
    void run();
    void close();
    void move(int plId, int direction);
    void addBomb(int id);
    //call this function when the game is launched
    void assignNumberToPlayers();
    void createRandomMap(int w, int h,int squareSize);
private:
    MapServer *map;
    int maxNbPlayer;
    int playerIdIncrement;
    int port;
    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;
    QList<NetServerClient *> clients;
    int readMove(QDataStream &in);
    void sendUdpWelcomeAck(QHostAddress sender, quint16 senderPort);
    void sendPingBack(quint32 cpt, QHostAddress sender, quint16 senderPort);
private slots:
    void incomingClient();
    void receiveUdp();
    void removeBomb(int bombId);
    void addFlame(Flame& flame);

signals:
    void newPlayer();
    void serverError();
    void serverReady();
};

#endif

