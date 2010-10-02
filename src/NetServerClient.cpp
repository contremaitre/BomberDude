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
#include "Map.h"
#include <QTcpSocket>
#include <QUdpSocket>

NetServerClient::NetServerClient(QTcpSocket *t, QUdpSocket *u, int id, NetServer *s) 
{
    tcpSocket = t;
    udpSocket = u;
    //Had to add DirectConnection, to avoid a Qobject / qthread parenting error.
    //need to check this (cf http://forum.qtfr.org/viewtopic.php?id=10104)
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(incomingTcpData()), Qt::DirectConnection);
    peerAddress = tcpSocket->peerAddress();
    peerUdpPort = tcpSocket->peerPort();
    playerId = id;
    playerNumber = -1;
    blockSize = 0;
    server = s;
    qDebug() << "new NetServerClient " << id << peerAddress;
}

void NetServerClient::setPlayerNumber(int n)
{
    playerNumber = n;
}

void NetServerClient::incomingTcpData()
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

void NetServerClient::handleMsg(QDataStream &in)
{
    qint16 msg_type;
    in >> msg_type;
    switch(msg_type)
    {
        case msg_move:
        {
            qint16 direction;
            in >> direction;
            server->move(playerNumber, direction);
        }
        break;
        default:
            //trash the message
            in.skipRawData(blockSize);
        break;

    }
}

void NetServerClient::playerMoved(int plId, int x, int y)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_moved;
    out << (qint16)plId;
    out << (qint16)x;
    out << (qint16)y;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}


void NetServerClient::bombAdded(int plId, int squareX, int squareY)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_bomb;
    out << (qint16)plId;
    out << (qint16)squareX;
    out << (qint16)squareY;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}
void NetServerClient::bombRemoved(int x,int y){
	QByteArray block;
	    QDataStream out(&block, QIODevice::WriteOnly);
	    out.setVersion(QDataStream::Qt_4_0);
	    out << (quint16)0;
	    out << (quint16)msg_rmbomb;
	    out << (qint16)x;
	    out << (qint16)y;
	    out.device()->seek(0);
	    out << (quint16)(block.size() - sizeof(quint16));
	    udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}

void NetServerClient::sendMap(const Map &map)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_map;
    out << map;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}

int NetServerClient::getId() const
{
    return playerId;
}

QHostAddress NetServerClient::getAddress() const
{
    return peerAddress;
}

quint16 NetServerClient::getPeerUdpPort() const
{
    return peerUdpPort;
}

NetServerClient::~NetServerClient()
{

}

