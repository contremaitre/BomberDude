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

#ifndef QTB_NETCLIENT_H
#define QTB_NETCLIENT_H


#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QTime>

#include "MapClient.h"


class NetClient : public QObject
{
    Q_OBJECT
public:
    NetClient();
    ~NetClient();
    void connectToServer(QString ip, int port);
    void sendMove(int direction);
    void sendPing();
    void sendBomb();
    void setMaxPlayers(int);
    void startGame();
    void sendPlayerData(const QString& playerName);
    void setAdminPasswd(const QString &);
    void stopServer();
    void selectMap(int direction);

private:
    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    quint16 serverPort;
    QHostAddress serverAddress;
    //timeout in case udp communication fails
    QTimer *timerCheckUdp;
    QTime *timePing;
    quint32 cptPing;
    quint32 lastPingAck;
    quint32 udpCpt;
    QString adminPasswd;
    bool udpAckOk;
    void sendUdpWelcome();
    void handleTcpMsg(QDataStream &);
    void udpGenericStream(QDataStream &);
    void setBlockSize(const QByteArray &, QDataStream &);
    void sendUdpDatagram(const QByteArray &);
    void sendVersionNumber();
    void sendAdminPasswd();
    int udpCheckCount;
    quint16 blockSize; //size of the current message
    MapClient *map; //store the map when the server sends it;
    MapClient *mapPreview; //store the map when the server sends it;

private slots:
    void readTcpMsgFromServer();
    void receiveUdp();
    void slotTcpConnected();
    void slotTcpError(QAbstractSocket::SocketError);
    void checkUdp();

signals:
    void mapReceived(MapClient*);
    void mapPreviewReceived(MapClient*);
    void sigUpdatePlayerData(qint32 playerId, QString playerName);

    void sigMapRandom();
    void sigConnected();
    void sigConnectionError();
    void sigStatPing(int);
    void sigStatPacketLoss(double);
    void sigIsServerAdmin();
    void sigMaxPlayersChanged(int);
    void sigMapWinner(qint8 playerId);
    void sigGameStarted();
};


#endif // QTB_NETCLIENT_H
