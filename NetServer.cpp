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
#include "Map.h"
#include "NetMessage.h"
#include <QtNetwork>

NetServer::NetServer(const Map *map, int port) : QThread()
{
    this->map = new Map;
    *this->map = *map;
    this->port = port;
    maxNbPlayer = map->getMaxNbPlayers();
    playerIdIncrement = 0;
    tcpServer = NULL;
    udpSocket = NULL;
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
    exec();
}

void NetServer::incomingClient()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    //clientConnection->disconnectFromHost();

    NetServerClient *client = new NetServerClient(clientConnection,playerIdIncrement++,this);
    clients.append(client);
    client->sendMap(*map);
    qDebug() << "NetServer new client " << clients.size();
    emit newPlayer();
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
            break;
        }
        in >> msg;
        switch(msg)
        {
        case msg_udp_welcome:
            sendUdpWelcomeAck(sender,senderPort);
            break;
        case msg_move:
        {
            int direction = readMove(in);
            //TODO : would it be usefull to use a qmap here ?
            //qDebug() << "cherche client" << sender << senderPort;
            foreach (NetServerClient *client, clients) {
                //qDebug() << client->getAddress() << client->getPeerUdpPort();
                if(client->getAddress() == sender && client->getPeerUdpPort() == senderPort)
                {
                    //qDebug() << "trouve";
                    move(client->getId(), direction);
                }
            }
        }
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

int NetServer::readMove(QDataStream &in)
{
    qint16 direction;
    in >> direction;
    //qDebug() << "NetServer readMove UDP" << direction;
    return direction;
}

void NetServer::move(int plId, int direction)
{
    bool ok = map->movePlayer(plId,direction);
    if(ok)
    {
        //send the move to the clients
        qint16 x,y;
        map->getPlayerPosition(plId, x, y);
        foreach (NetServerClient *client, clients) {
            client->playerMoved(plId,x,y);
        }
    }
}

void NetServer::assignNumberToPlayers()
{
    int id = 0;
    foreach (NetServerClient *client, clients) {
        client->setPlayerNumber(id++);
    }
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
}

