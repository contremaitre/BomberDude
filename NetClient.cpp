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

#include "NetClient.h"

NetClient::NetClient()
{
    qDebug("new NetClient");
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMsgFromServer()));
}

void NetClient::connectToServer(QString ip, int port)
{
    tcpSocket->connectToHost(ip,port);
}

void NetClient::readMsgFromServer()
{
    qDebug("NetClient incomming message");
}

NetClient::~NetClient()
{
    delete tcpSocket;
}

