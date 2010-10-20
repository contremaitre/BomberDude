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
#include "constant.h"
#include "NetServerClient.h"
#include "MapServer.h"
#include "NetMessage.h"
#include <QtNetwork>
#include <unistd.h> // for usleep

NetServer::NetServer(int port) : QThread()
{
    map = new MapServer;
    connect(map,SIGNAL(updatedMap(QByteArray)),this,SLOT(updateMap(QByteArray)));
    this->port = port;
    tcpServer = NULL;
    udpSocket = NULL;
    playersInGame = NULL;
}

void NetServer::run()
{
    qDebug("NetServer run");
    tcpServer = new QTcpServer();
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "server tcp error :" << tcpServer->errorString();
        emit serverError();
        return;
    }
    udpSocket = new QUdpSocket();
    udpSocket->bind(QHostAddress::Any, port);

    //hack ? cf NetServerClient::NetServerClient()
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(receiveUdp()), Qt::DirectConnection);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(incomingClient()), Qt::DirectConnection);
    emit serverReady();
	map->startHeartBeat(0, MOVE_TICK_INTERVAL);
    exec();
}

void NetServer::incomingClient()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();

    int playerId = -1;
    for(int i = 0; i < map->getMaxNbPlayers(); i++)
    {
        if(playersInGame[i] == -1)
        {
            playerId = i;
            playersInGame[i] = playerId;
            break;
        }
    }
    if(playerId != -1)
    {
        NetServerClient *client = new NetServerClient(clientConnection,udpSocket,playerId,this);
        connect(client, SIGNAL(disconected(NetServerClient*)), this, SLOT(clientDisconected(NetServerClient*)));
        clients.append(client);
        client->sendMap(*map);
        qDebug() << "NetServer new client " << clients.size();
        emit newPlayer();
    }
    else
    {
        qDebug() << "New client. No place left";
        clientConnection->disconnectFromHost();
    }
}

void NetServer::clientDisconected(NetServerClient *client)
{
    qDebug("NetServer : clientDisconected");
    for (int i = 0; i < clients.size(); ++i) {
        if (clients.at(i) == client)
        {
            clients.removeAt(i);
            playersInGame[client->getId()] = -1;
            delete client;
            if(clients.empty())
                emit allPlayersLeft();
            return;
        }
    }
    qDebug("didn't find it");
}

void NetServer::receiveUdp()
{
    //qDebug() << "NetServer receive udp";
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
            qDebug() << "NetServer readMove size error" << datagram.size() << size;
            continue;
        }
        in >> msg;
        NetServerClient *client = NULL;
        //TODO : would it be usefull to use a qmap here ?
        //qDebug() << "cherche client" << sender << senderPort;
        foreach (NetServerClient *cl, clients) {
            if(cl->getAddress() == sender && cl->getPeerUdpPort() == senderPort)
            {
                //qDebug() << "trouve";
                client = cl;
            }
        }
        if(!client)
        {
            qDebug("Received a udp message from an unknown client !");
            continue;
        }
        client->udpReceived();
        switch(msg)
        {
        case msg_udp_welcome:
            sendUdpWelcomeAck(sender,senderPort);
            break;
        case msg_move:
        {
            int direction = readMove(in);
			map->requestMovePlayer(client->getId(), direction);
        }
            break;
        case msg_ping:
        {
          //simulate 150ms latency :
	  //usleep(150*1000);
          quint32 cpt;
          in >> cpt;
	  sendPingBack(cpt, sender,senderPort);
          break;
        }
        case msg_bomb:
			map->requestBombPlayer(client->getId());
			break;
        default:
            qDebug() << "NetServer readMove discarding unkown message";
            break;
        }

    }
}

void NetServer::sendUdpWelcomeAck(QHostAddress sender, quint16 senderPort)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_udp_welcome;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    udpSocket->writeDatagram(block,sender,senderPort);
    //qDebug() << "NetServer sendUdpWelcome";
}


void NetServer::sendPingBack(quint32 cpt, QHostAddress sender, quint16 senderPort)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_ping;
    out << cpt;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    udpSocket->writeDatagram(block,sender,senderPort);
    //qDebug() << "NetServer sendPingBack" << cpt;
}

int NetServer::readMove(QDataStream &in)
{
    qint16 direction;
    in >> direction;
    //qDebug() << "NetServer readMove UDP" << direction;
    return direction;
}

void NetServer::createRandomMap(int w, int h,int squareSize)
{
    if(!clients.empty())
        qFatal("create map, and player already in game");
    qDebug() << "going to create random map";
    map->setDim(w,h,squareSize);
    qDebug() << "set Dimensions "<<w<<" "<<h<<" "<<squareSize;
    map->loadRandom();
    qDebug() << "random map created";
    delete[] playersInGame;
    playersInGame = new int[map->getMaxNbPlayers()];
    for(int i = 0; i < map->getMaxNbPlayers(); i++)
        playersInGame[i] = -1;
}

NetServer::~NetServer()
{
    foreach (NetServerClient *client, clients) {
        delete client;
    }
    clients.clear();
    delete tcpServer;
    delete udpSocket;
    delete map;
    delete[] playersInGame;
}

void NetServer::updateMap(QByteArray updateData) {
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << (quint16)0;
	out << (quint16)msg_update_map;
	out << updateData;
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));

	foreach(NetServerClient *client, clients)
		client->sendUpdate(block);
}
