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
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(clientDisconected()));
    peerAddress = tcpSocket->peerAddress();
    peerUdpPort = tcpSocket->peerPort();
    playerId = id;
    playerNumber = -1;
    blockSize = 0;
    udpCpt = 0;
    packetErrors = 0;
    lastReceivedPckt = 0;
    server = s;
    qDebug() << "new NetServerClient " << id << peerAddress;
}

void NetServerClient::clientDisconected()
{
    emit disconected(this);
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
    case msg_net_version:
    {
        qint16 version;
        in >> version;
        //qDebug() << "NetServerClient, received client version number : " << version;
        if(version != NET_VERSION)
        {
            qDebug() << "version mismatch (" << version << NET_VERSION << ")";
            tcpSocket->close();
        }
    }
    break;
    default:
        //trash the message
        qDebug() << "NetServerClient, unexpected tcp message received" << msg_type;
        in.skipRawData(blockSize);
        break;

    }
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

void NetServerClient::sendUdpStats()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_udp_stat;
    out << packetErrors;
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

void NetServerClient::udpReceived(quint32 pckNum)
{
    if(pckNum != lastReceivedPckt + 1)
    {
        qDebug() << "NetServerClient, udp receive number mismatch" << pckNum << lastReceivedPckt + 1;
        packetErrors++;
    }
    lastReceivedPckt = pckNum;
    udpCpt++;
    if(udpCpt % UDP_STATS_INTERVAL == 0)
    {
        //qDebug() << "player" << playerId << "received" << udpCpt << "udp packets";
        sendUdpStats();
        packetErrors = 0;
    }
}

void NetServerClient::sendUpdate(const QByteArray& block) {
	//qDebug() << "Sending update";
	udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}

NetServerClient::~NetServerClient()
{
    tcpSocket->deleteLater();
}

