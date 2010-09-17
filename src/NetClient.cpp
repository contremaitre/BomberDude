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
#include <QUdpSocket>
#include <QTimer>
#include <QTime>

NetClient::NetClient()
{
    timePing = new QTime();
    timePing->start();
    cptPing = 0;
    lastPingAck = 0;
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(slotTcpConnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotTcpError(QAbstractSocket::SocketError)));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMsgFromServer()));
    udpSocket = new QUdpSocket(); 
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(receiveUdp()));
    blockSize = 0;
    map = NULL;
    serverAddress.setAddress("");
    serverPort = 0;
    timerCheckUdp = NULL;
    
}

void NetClient::connectToServer(QString ip, int port)
{
    tcpSocket->connectToHost(ip,port);
}

void NetClient::sendUdpWelcome()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_udp_welcome;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    //qDebug() << "NetClient sendUdpWelcome" << udpSocket->localPort() << udpSocket->peerPort();
    udpSocket->writeDatagram(block,serverAddress,serverPort);
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
    //tcpSocket->write(block);
    //qDebug() << "NetClient send move udp" << serverAddress << serverPort;
    udpSocket->writeDatagram(block, serverAddress, serverPort);
}


void NetClient::sendPing()
{
    if(lastPingAck != cptPing)
        qDebug() << "Ping : timeout";
    cptPing++;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_ping;
    out << cptPing;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    udpSocket->writeDatagram(block, serverAddress, serverPort);
    timePing->restart();
}


void NetClient::receiveUdp()
{
  //qDebug() << "NetClient receive udp";
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QDataStream in(&datagram, QIODevice::ReadOnly);
        quint16 size, msg;
        in >> size;
        if(datagram.size() - sizeof(size) != size)
        {
            qDebug() << "NetClient readMove size error" << datagram.size() << size;
            break;
        }
        in >> msg;
        switch(msg)
        {
        case msg_udp_welcome:
            timerCheckUdp->stop();
            if(!udpAckOk)
            {
                udpAckOk = true;
                emit sigConnected();
            }
            break;
	case msg_ping: 
            in >> lastPingAck;
            if(lastPingAck == cptPing)
            {
                int e = timePing->elapsed();
                qDebug() << "Ping :" << e; 
            }
            else
                qDebug() << "Ping : received out of delay";
            break;
	default:
            qDebug() << "NetClient readMove discarding unkown message";
            break;
        }
    }   
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
            qint16 player, x, y;
            in >> player >> x >> y;
            //qDebug() << "netclient move received " << x << " " << y;
            emit moveReceived( player, x, y );
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

void NetClient::checkUdp()
{
    if(udpCheckCount>=10)//todo configurable
    {
        timerCheckUdp->stop();
        qDebug() << "NetClient : diddn't receive udp ack. Check you firewall/router";
        emit sigConnectionError();
        return;
    }
    udpCheckCount++;
    sendUdpWelcome();
}

void NetClient::slotTcpConnected()
{
    serverPort = tcpSocket->peerPort();
    serverAddress = tcpSocket->peerAddress();
    bool b = udpSocket->bind(tcpSocket->localPort());
    if(b)
    {
        //qDebug() << "NetClient slotTcpConnected" << serverAddress << serverPort;
        udpCheckCount = 0;
        udpAckOk = false;
        timerCheckUdp = new QTimer(this);
        checkUdp();
        connect(timerCheckUdp, SIGNAL(timeout()),this,SLOT(checkUdp()));
        timerCheckUdp->start(1000);//todo configurable ?
        //emit sigConnected();
    }
    else
    {
        qDebug() << "NetClient udp bind error " << udpSocket->error();
        emit sigConnectionError();
    }
}

void NetClient::slotTcpError(QAbstractSocket::SocketError error)
{
    qDebug() << "NetClient tcp error " << error;
    emit sigConnectionError();
}

NetClient::~NetClient()
{
    delete timerCheckUdp;
    delete udpSocket;
    delete tcpSocket;
    delete map;
}

