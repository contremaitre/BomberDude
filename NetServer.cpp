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
#include <QtNetwork>
#include "constant.h"
#include "NetServerClient.h"

NetServer::NetServer() : QThread()
{
}

void NetServer::run()
{
    qDebug("NetServer run");
    tcpServer = new QTcpServer();
    if (!tcpServer->listen(QHostAddress::Any, SERVER_PORT)) {
        //TODO error
        return;
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(incomingClient()));
    exec();
}

void NetServer::incomingClient()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    //clientConnection->write(block);
    //clientConnection->disconnectFromHost();

    NetServerClient *client = new NetServerClient;
    clients << client;
}

NetServer::~NetServer()
{
    foreach (NetServerClient *client, clients) {
        delete client;
    }
    clients.clear();
    delete tcpServer;
}

