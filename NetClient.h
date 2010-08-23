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

class QTcpSocket;
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
    void handleMsg(QDataStream &);
    quint16 blockSize; //size of the current message
    Map *map; //store the map when the server sends it;

private slots:
    void readMsgFromServer();

signals:
    void moveReceived(int plId,int pos);
    void mapReceived(const Map *);
};

#endif

