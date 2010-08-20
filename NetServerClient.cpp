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

#include "NetServerClient.h"
#include "NetServer.h"
#include "NetMessage.h"
#include <QTcpSocket>

NetServerClient::NetServerClient(QTcpSocket *t, int id, NetServer *s) 
{
    tcpSocket = t;
    //Had to add DirectConnection, to avoid a Qobject / qthread parenting error.
    //need to check this (cf http://forum.qtfr.org/viewtopic.php?id=10104)
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(incomingData()), Qt::DirectConnection);
    playerId = id;
    playerNumber = -1;
    server = s;
    qDebug() << "new NetServerClient " << id;
}

void NetServerClient::setPlayerNumber(int n)
{
    playerNumber = n;
}

void NetServerClient::incomingData()
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

void NetServerClient::handleMsg(NetHeader *msg)
{
    //qDebug() << "NetServerClient::handleMsg, type = " << msg->type;
    switch(msg->type)
    {
        case msg_move:
            server->move(playerNumber, ((NetMsgMove*)msg)->direction);
    }
}

void NetServerClient::playerMoved(int plId, int position)
{
    qDebug("NetServerClient player moved");
    NetMsgMoved msg;
    msg.type = msg_moved;
    msg.length = sizeof(msg);
    msg.position = position;
    msg.player = plId;
    tcpSocket->write((const char*)&msg, sizeof(msg));
    qDebug("NetServerClient player moved ok");
}

NetServerClient::~NetServerClient()
{

}

