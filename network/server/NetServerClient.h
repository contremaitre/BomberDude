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

#ifndef QTB_NETSERVERCLIENT_H
#define QTB_NETSERVERCLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QList>
#include <QPoint>

#include "Flame.h"
#include "Map.h"
#include "PlayerServer.h"


class NetServer;

class NetServerClient : public QObject
{
    Q_OBJECT

      public:
  NetServerClient(QTcpSocket *, QUdpSocket *, int id, bool admin, int maxPl, NetServer *);
  ~NetServerClient();
  void sendMap(const Map<PlayerServer>&);
  void sendMapPreview(const Map<PlayerServer>&);
  void sendUpdate(const QByteArray& block);
  void sendTcpBlock(const QByteArray& block);
  QHostAddress getAddress() const;
  quint16 getPeerUdpPort() const;

  int getId() const;
  const QString& getPlayerName() const      { return playerName; }

  void udpReceived(quint32 pckNum);
  void sendUdpStats();
  void sendMaxPlayers(int);
  void setAdmin();

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
  void sendIsAdmin();

  int playerId;
  int playerNumber;
  QString playerName;

  quint16 blockSize; //size of the current message

private slots:
  void incomingTcpData();
  void clientDisconected();

signals:
    void disconected(NetServerClient*);
    void sigUpdatePlayerData(int playerId, QString playerName);
};


#endif // QTB_NETSERVERCLIENT_H
