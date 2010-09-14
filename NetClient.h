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
 * This class communicates with the server
 * 
 * It can request a move to the server, and receive other players movement from the server
 *
 */

#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QHostAddress>

class QTcpSocket;
class QUdpSocket;
class QTimer;

class Map;

typedef struct NetHeader NetHeader;

class NetClient : public QObject
{
    Q_OBJECT
public:
    NetClient();
    ~NetClient();
    void connectToServer(QString ip, int port);
    void sendMove(int direction);

private:
    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    quint16 serverPort;
    QHostAddress serverAddress;
    //timeout in case udp communication fails
    QTimer *timerCheckUdp;
    bool udpAckOk;
    void sendUdpWelcome();
    void handleMsg(QDataStream &);
    int udpCheckCount;
    quint16 blockSize; //size of the current message
    Map *map; //store the map when the server sends it;

private slots:
    void readMsgFromServer();
    void receiveUdp();
    void slotTcpConnected();
    void slotTcpError(QAbstractSocket::SocketError);
    void checkUdp();
signals:
    void moveReceived(qint16 plId, qint16 x, qint16 y);
    void mapReceived(const Map *);
    void sigConnected();
    void sigConnectionError();
};

#endif

