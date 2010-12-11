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

#include <QtNetwork>
#include <QDir>

#include "MapServer.h"
#include "NetServer.h"
#include "constant.h"
#include "../NetMessage.h"
#include "MapParser.h"

NetServer::NetServer(int port, QString adminPasswd, bool debugMode) : QThread()
{
    map = NULL;
    mapPreview = NULL;
    /* default random map size*/
    mapW = MAP_SIZE;
    mapH = MAP_SIZE;
    blockSize = BLOCK_SIZE;
    this->port = port;
    this->adminPasswd = adminPasswd;
    tcpServer = NULL;
    udpSocket = NULL;
    QDir mapDirectory(MAP_PATH);
    mapList = mapDirectory.entryInfoList(QStringList("*.xml"));
    currentMapInList = 0;
    gameStarted = false;
    maxNbPlayers = 2; //1
    adminConnected = false;
    randomMap = false;
    this->debugMode = debugMode;
    selectMap(2);
    connect(this,SIGNAL(sigStartHeartBeat()), this, SLOT(startHeartBeat()), Qt::QueuedConnection);
}

void NetServer::run()
{
    qDebug() << "NetServer run, admin passord : " << adminPasswd;
    tcpServer = new QTcpServer();
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "server tcp error :" << tcpServer->errorString();
        emit sigQuit();
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

void NetServer::startHeartBeat() {
    qDebug() << "NetServer::startHeartBeat";

    // starts with a negative heartbeat for a countdown
    qint32 initHB = debugMode ? 0 : -2 * (1000 / MOVE_TICK_INTERVAL);
	map->startHeartBeat(initHB, MOVE_TICK_INTERVAL);
}

void NetServer::setBlockSize(const QByteArray &block, QDataStream & out)
{
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
}

void NetServer::incomingClient()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    qDebug() << "NetServer::incomingClient" << map;
    if(clients.size() < maxNbPlayers)
    {
        int freeIndex;
        //search for the minimum free index in players list
        for (freeIndex = 0; freeIndex < clients.size(); ++freeIndex)
        {
            if (clients.at(freeIndex)->getId() > freeIndex)
            {
                //we will insert the client here
                break;
            }
        }
        Q_ASSERT(freeIndex < maxNbPlayers);

        //if freeIndex == 0 && !adminPasswd, this client will be the admin of the server
        NetServerClient *client = new NetServerClient(clientConnection,udpSocket,freeIndex, freeIndex == 0 && adminPasswd.isEmpty(), maxNbPlayers, this);
        connect(client, SIGNAL(disconected(NetServerClient*)), this, SLOT(clientDisconected(NetServerClient*)));
        connect(client, SIGNAL(sigUpdatePlayerData(int,QString)), this, SLOT(slotUpdatePlayerData(int,QString)));

        clients.insert(freeIndex,client);
        if(gameStarted) //we allow clients to join a game already started
            client->sendMap(*map);
        qDebug() << "NetServer new client " << client->getId() << clients.size();

        // we send him the list of all clients (including himself) already connected
        foreach(NetServerClient* Nclient, clients) {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << static_cast<quint16>(0);
            out << static_cast<quint16>(msg_update_player_data);
            out << static_cast<qint32>(Nclient->getId());
            out << Nclient->getPlayerName();
            setBlockSize(block, out);
            client->sendTcpBlock(block);
        }
        //send the current map preview
        if(!randomMap && mapPreview != NULL)
        {
            foreach(NetServerClient *client, clients)
                client->sendMapPreview(mapPreview);
        }

        emit newPlayer();
    }
    else
    {
        qDebug() << "New client. No place left";
        clientConnection->disconnectFromHost();
    }
}

void NetServer::shutdown()
{
    emit sigQuit();
}

bool NetServer::passwordReceived(int id, QString &pass)
{
    if(adminConnected)
        return false;
    if(!adminPasswd.isEmpty() && pass == adminPasswd )
    {
        foreach(NetServerClient *client, clients)
        {
            if(client->getId() == id)
            {
                adminConnected = true;
                client->setAdmin();
                return true;
            }
        }
    }
    return false;
}

void NetServer::startGame()
{
    if(!gameStarted)
    {
        loadMap();
        foreach(NetServerClient *client, clients)
            map->assignPlayer(client->getId());
        foreach(NetServerClient *client, clients)
            client->sendMap(*map);
        gameStarted = true;
    }
}

void NetServer::selectMap(qint8 direction)
{
    qDebug() << "NetServer selectMap" << direction << currentMapInList;
    if(gameStarted)
    {
        qDebug("NetServer selectMap and game already started");
        return;
    }
    if(mapList.isEmpty())
    {
        qDebug("no map available, force random");
        randomMap = true;
        return;
    }

    if(direction == 2)
    {
        //switch to non random map and select the first available map
        if(!randomMap && mapPreview && maxNbPlayers <= mapPreview->getMaxNbPlayers())
        {
            //no change
            return;
        }
        qDebug() << "NetServer non random map";
        randomMap = false;
        currentMapInList--;
        direction = 1;
    }
    if(direction == -1 || direction == 1)
    {
        //next or previous map
        currentMapInList += direction;
        if(currentMapInList < 0)
            currentMapInList = mapList.size()-1;
        if(currentMapInList >= mapList.size())
            currentMapInList = 0;
        int firstChecked = currentMapInList;
        bool found = false;
        do
        {
            qDebug() << "going to load map" << mapList[currentMapInList].fileName();
            delete mapPreview;
            mapPreview = new MapServer;
            MapParser mapParser(mapPreview);
            QFile mapXmlFile(mapList[currentMapInList].absoluteFilePath());
            QXmlInputSource source(&mapXmlFile);
            // Create the XML file reader
            QXmlSimpleReader reader;
            reader.setContentHandler(&mapParser);
            // Parse the XML file
            reader.parse(source);
            qDebug() << "map loaded, current max players" << maxNbPlayers << "map max player" << mapPreview->getMaxNbPlayers();
            if(maxNbPlayers <= mapPreview->getMaxNbPlayers())
            {
                foreach(NetServerClient *client, clients)
                    client->sendMapPreview(mapPreview);
                found = true;
                break;
            }
            currentMapInList += direction;
            if(currentMapInList < 0)
                currentMapInList = mapList.size()-1;
            if(currentMapInList >= mapList.size())
                currentMapInList = 0;
        }while(currentMapInList != firstChecked);
        if(!found)
        {
            qDebug() << "No map found, go random";
            randomMap = true;
        }
    }
    else if(direction == 0)
    {
        //random map
        qDebug() << "NetServer random map";
        randomMap = true;
    }
    if(randomMap)
    {
        foreach(NetServerClient *client, clients)
            client->sendMapRandom();
    }
}

void NetServer::setMaxPlayers(int value)
{
    maxNbPlayers = value;
    //disconnect some clients if too many
    while( clients.size() > maxNbPlayers )
    {
        qDebug() << "setMaxPlayers, too many players, disconnecting one";
        NetServerClient *last = clients.takeLast();
        delete last;

    }

    foreach(NetServerClient *client, clients)
        client->sendMaxPlayers(maxNbPlayers);
    if(!randomMap)
        selectMap(2); //check if the current map is ok with this number of player
}

void NetServer::clientDisconected(NetServerClient *client)
{
    qDebug("NetServer : clientDisconected");
    for (int i = 0; i < clients.size(); ++i) {
        if (clients.at(i) == client)
        {
            clients.removeAt(i);
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
        quint32 pckCount;
        in >> size;
        if(datagram.size() - sizeof(size) != size)
        {
            qDebug() << "NetServer udp packet size read error" << datagram.size() << size;
            continue;
        }
        in >> pckCount;
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
        client->udpReceived(pckCount);
        switch(msg)
        {
        case msg_udp_welcome:
            sendUdpWelcomeAck(sender,senderPort);
            break;
        case msg_move:
        {
            int direction = readMove(in);
            if(gameStarted)
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
            if(gameStarted)
                map->requestBombPlayer(client->getId());
			break;
        default:
            qDebug() << "NetServer readMove discarding unkown message";
            break;
        }

    }
}

void NetServer::slotUpdatePlayerData(int playerId, QString playerName) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << static_cast<quint16>(0);
    out << static_cast<quint16>(msg_update_player_data);
    out << static_cast<qint32>(playerId);
    out << playerName;
    setBlockSize(block, out);

    foreach(NetServerClient* Nclient, clients)
        Nclient->sendTcpBlock(block);
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

void NetServer::allocMap()
{
    delete map;
    map = new MapServer;
    connect(map,SIGNAL(updatedMap(QByteArray)),this,SLOT(updateMap(QByteArray)));
    emit sigStartHeartBeat();
    //connect(this, SIGNAL(started()), this, SLOT(startHeartBeat()));
}

bool NetServer::loadMap()
{
    if(gameStarted)
        qFatal("create map, and game already started");

    allocMap();
    map->setDebugMode(debugMode);
    qDebug() << "loadMap :" << currentMapInList << mapList.size();
    if(!randomMap && !mapList.isEmpty() && currentMapInList >= 0 && currentMapInList < mapList.size()) //file
    {
        qDebug() << "going to load map" << mapList[currentMapInList].fileName();
        MapParser mapParser(map);
        QFile mapXmlFile(mapList[currentMapInList].absoluteFilePath());
        QXmlInputSource source(&mapXmlFile);
        // Create the XML file reader
        QXmlSimpleReader reader;
        reader.setContentHandler(&mapParser);
        // Parse the XML file
        reader.parse(source);
        qDebug() << "map loaded";
    }
    else //random
    {
        qDebug() << "set Dimensions "<< mapW << mapH << blockSize;
        map->setDim(mapW,mapH,blockSize);
        map->loadRandom();
    }
    return true;
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
    delete mapPreview;
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
