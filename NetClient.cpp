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
#include "NetMessage.h"

#include <QTcpSocket>

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

void NetClient::sendMove(int direction)
{
    NetMsgMove msg;
    msg.type = msg_move;
    msg.length = sizeof(msg);
    msg.direction = direction;
    tcpSocket->write((const char*)&msg, sizeof(msg));
}

void NetClient::readMsgFromServer()
{
    char buff[1000];
    NetHeader *nh = (NetHeader*)buff;
    quint64 ret = tcpSocket->read(buff, sizeof(NetHeader));
    if(ret == sizeof(NetHeader))
    {
        ret = tcpSocket->read(buff + sizeof(NetHeader), nh->length - sizeof(NetHeader));
        if(ret == (nh->length - sizeof(NetHeader)))
            handleMsg(nh);
    }
}

void NetClient::handleMsg(NetHeader *msg)
{
    //qDebug() << "NetClient::handleMsg, type = " << msg->type;
    switch(msg->type)
    {
        case msg_moved:
        {
            //qDebug("netclient move received");
            emit moveReceived( ((NetMsgMoved*)msg)->player,((NetMsgMoved*)msg)->position );
        }
    }
}

NetClient::~NetClient()
{
    delete tcpSocket;
}

