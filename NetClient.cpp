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
#include "Map.h"

#include <QTcpSocket>

NetClient::NetClient()
{
    qDebug("new NetClient");
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMsgFromServer()));
    blockSize = 0;
    map = NULL;
}

void NetClient::connectToServer(QString ip, int port)
{
    tcpSocket->connectToHost(ip,port);
}

void NetClient::sendMove(int direction)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_move;
    out << (qint16)direction;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}

void NetClient::readMsgFromServer()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }

    if (tcpSocket->bytesAvailable() < blockSize)
        return;
    handleMsg(in);
    blockSize = 0;
}

void NetClient::handleMsg(QDataStream &in)
{
    qint16 msg_type;
    in >> msg_type;
    //qDebug() << "NetClient::handleMsg, type = " << msg->type;
    switch(msg_type)
    {
        case msg_moved:
        {
            qint16 player, position;
            in >> player >> position;
            //qDebug("netclient move received");
            emit moveReceived( player,position );
        }
        break;
        case msg_map:
            qDebug("NetClient map received");
            map = new Map;
            in >> *map;
            emit mapReceived(map);
        //break;
        default:
            //trash the message
            in.skipRawData(blockSize);
        break;
    }
}

NetClient::~NetClient()
{
    delete tcpSocket;
    delete map;
}

