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
        case msg_move:
        {
            qint16 direction;
            in >> direction;
			//server->move(playerNumber, direction);
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


void NetServerClient::bombAdded(int plId, int squareX, int squareY, int bombId)
{
    qDebug()<<"send bomb to client";
	QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_bomb;
    out << (qint16)plId;
    out << (qint16)squareX;
    out << (qint16)squareY;
    out << (qint16)bombId;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}
void NetServerClient::bombRemoved(int bombId){
	QByteArray block;
	    QDataStream out(&block, QIODevice::WriteOnly);
	    out.setVersion(QDataStream::Qt_4_0);
	    out << (quint16)0;
	    out << (quint16)msg_rmbomb;
	    out << (qint16)bombId;
	    out.device()->seek(0);
	    out << (quint16)(block.size() - sizeof(quint16));
	    udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}

void NetServerClient::flameRemoved(int flameId){
	QByteArray block;
		    QDataStream out(&block, QIODevice::WriteOnly);
		    out.setVersion(QDataStream::Qt_4_0);
		    out << (quint16)0;
		    out << (quint16)msg_rmflame;
		    out << (qint16)flameId;
		    out.device()->seek(0);
		    out << (quint16)(block.size() - sizeof(quint16));
		    udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}

void NetServerClient::flameAdded(Flame & flame ){
	QByteArray block;
		    QDataStream out(&block, QIODevice::WriteOnly);
		    out.setVersion(QDataStream::Qt_4_0);
		    out << (quint16)0;
		    out << (quint16)msg_flame;
		    out << flame;
		    out.device()->seek(0);
		    out << (quint16)(block.size() - sizeof(quint16));
		    udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
		    //qDebug()<<"NetServerClient> flameAdded "<<flame.getFlamePositions().size()<<" "<<flame.getFlamePositions().at(0)->x()<<" "<<flame.getFlamePositions().at(0)->y() ;
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
    out << udpCpt;
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

void NetServerClient::udpReceived()
{
    udpCpt++;
    if(udpCpt % 100 == 0)
    {
        //qDebug() << "player" << playerId << "received" << udpCpt << "udp packets";
        sendUdpStats();
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

