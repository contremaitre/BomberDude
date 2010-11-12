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
* Keep track of each client connected to the server
*
*/

#ifndef NETSERVERCLIENT_H
#define NETSERVERCLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QList>
#include <QPoint>
#include "Flame.h"
class QTcpSocket;
class QUdpSocket;
class NetServer;
class Map;

typedef struct NetHeader NetHeader;

class NetServerClient : public QObject
{
    Q_OBJECT

      public:
  NetServerClient(QTcpSocket *, QUdpSocket *, int id, bool admin, int maxPl, NetServer *);
  ~NetServerClient();
  void sendMap(const Map&);
  void sendUpdate(const QByteArray& block);
  QHostAddress getAddress() const;
  quint16 getPeerUdpPort() const;
  int getId() const;
  void udpReceived(quint32 pckNum);
  void sendUdpStats();
  void sendMaxPlayers(int);

 private:
  QTcpSocket *tcpSocket;
  QUdpSocket *udpSocket;
  NetServer *server;
  QHostAddress peerAddress;
  quint16 peerUdpPort;
  quint32 udpCpt;
  quint32 lastReceivedPckt;
  quint16 packetErrors; //udp packet errors since last report
  bool isAdmin;
  void handleMsg(QDataStream &);
  void sendIsAdmin(int max);
  int playerId;
  int playerNumber;
  quint16 blockSize; //size of the current message

private slots:
  void incomingTcpData();
  void clientDisconected();

signals:
    void disconected(NetServerClient*);
};

#endif

