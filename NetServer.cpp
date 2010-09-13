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

#include "NetServer.h"
#include "constant.h"
#include "NetServerClient.h"
#include "Map.h"
#include "NetMessage.h"
#include <QtNetwork>

NetServer::NetServer(const Map *map, int port) : QThread()
{
    this->map = new Map;
    *this->map = *map;
    this->port = port;
    maxNbPlayer = map->getMaxNbPlayers();
    playerIdIncrement = 0;
    tcpServer = NULL;
}

void NetServer::run()
{
    qDebug("NetServer run");
    tcpServer = new QTcpServer();
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "server tcp error :" << tcpServer->errorString();
        emit serverError();
        return;
    }
    //hack ? cf NetServerClient::NetServerClient()
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(incomingClient()), Qt::DirectConnection);
    emit serverReady();
    exec();
}

void NetServer::incomingClient()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    //clientConnection->disconnectFromHost();

    NetServerClient *client = new NetServerClient(clientConnection,playerIdIncrement++,this);
    clients.append(client);
    client->sendMap(*map);
    qDebug() << "new client " << clients.size();
    emit newPlayer();
}

void NetServer::move(int plId, int direction)
{
    bool ok = map->movePlayer(plId,direction);
    if(ok)
    {
        //send the move to the clients
        qint16 x,y;
        map->getPlayerPosition(plId, x, y);
        foreach (NetServerClient *client, clients) {
            client->playerMoved(plId,x,y);
        }
    }
}

void NetServer::assignNumberToPlayers()
{
    int id = 0;
    foreach (NetServerClient *client, clients) {
        client->setPlayerNumber(id++);
    }
}

NetServer::~NetServer()
{
    foreach (NetServerClient *client, clients) {
        delete client;
    }
    clients.clear();
    delete tcpServer;
    delete map;
}

