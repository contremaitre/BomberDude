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
#include "../NetMessage.h"


NetServerClient::NetServerClient(QTcpSocket *t, QUdpSocket *u, int id, bool admin, int maxPl, NetServer *s)
{
    tcpSocket = t;
    udpSocket = u;
    isAdmin = admin;
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
    sendMaxPlayers(maxPl);
    if(admin)
        sendIsAdmin();
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

    do {
        if (blockSize == 0) {
            if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
                return;
            in >> blockSize;
        }
    
        if (tcpSocket->bytesAvailable() < blockSize)
            return;
        handleMsg(in);
        blockSize = 0;
    } while(! tcpSocket->atEnd());
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
        break;
    }
    case msg_max_players:
    {
        quint16 value;
        in >> value;
        if(isAdmin)
            server->setMaxPlayers(value);
        break;
    }
    case msg_start_game:
    {
        qint8 styleIndex;
        in >> styleIndex;
        if(isAdmin)
            server->startGame(styleIndex);
        break;
    }
    case msg_select_map:
    {
        qint8 direction;
        in >> direction;
        if(isAdmin)
            server->selectMap(direction);
        break;
    }
    case msg_kick_player :
    {
        qint8 playerId;
        in >> playerId;
        if(isAdmin)
            server->kickPlayer(playerId);
        break;
    }
    case msg_player_data: {
            QString name;
            in >> name;
            if(name.size() <= MAX_PLAYER_NAME_LENGTH) {
                playerName = name;
                emit sigUpdatePlayerData(playerId, playerName);
            }
            //qDebug() << "Player id: " << playerId << " name: " << name;
        }
        break;

    case msg_admin_passwd:
    {
        QString pass;
        in >> pass;
        if(!server->passwordReceived(playerId,pass))
            tcpSocket->close();
        break;
    }
    case msg_shutdown_server:
        qDebug() << "NetServerClient msg_shutdown_server" << isAdmin;
        if(isAdmin)
            server->shutdown();
        break;
    default:
        //trash the message
        qDebug() << "NetServerClient, unexpected tcp message received" << msg_type;
        in.skipRawData(blockSize - sizeof(qint16));      // blockSize contains the message type and we already popped it
        break;

    }
}

void NetServerClient::setAdmin()
{
    isAdmin = true;
    sendIsAdmin();
}

void NetServerClient::sendMaxPlayers(int value)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_max_players;
    out << (quint16)value;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}

void NetServerClient::sendMapRandom(bool rand)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_map_random;
    out << rand;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}

void NetServerClient::sendMapPreview(const Map<PlayerServer,BombServer,mapStyle> *map)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_map_preview;
    out << *map;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}

void NetServerClient::sendMap(const Map<PlayerServer,BombServer,mapStyle>& map)
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
    //qDebug() << "NetServerClient sending map to client " << playerId;
}

void NetServerClient::sendGameStarted()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_start_game;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
    //qDebug() << "NetServerClient sending map to client " << playerId;
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

void NetServerClient::sendIsAdmin()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_is_admin;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}

void NetServerClient::sendUpdate(const QByteArray& block) {
	//qDebug() << "Sending update";
	udpSocket->writeDatagram(block,peerAddress,peerUdpPort);
}

void NetServerClient::sendTcpBlock(const QByteArray& block) {
    tcpSocket->write(block);
}

NetServerClient::~NetServerClient()
{
    tcpSocket->deleteLater();
}

