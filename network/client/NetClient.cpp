/*
    Copyright (C) 2010,2011 Sébastien Escudier

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
#include "../NetMessage.h"


NetClient::NetClient()
{
	timePing = new QTime();
	timePing->start();
	cptPing = 0;
    isAdmin = false;
	udpCpt = 0;
	lastPingAck = 0;
	tcpSocket = new QTcpSocket();
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(slotTcpConnected()));
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotTcpError(QAbstractSocket::SocketError)));
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readTcpMsgFromServer()));
	udpSocket = new QUdpSocket();
	connect(udpSocket, SIGNAL(readyRead()), this, SLOT(receiveUdp()));
	blockSize = 0;
	map = NULL;
	serverAddress.setAddress("");
	serverPort = 0;
	timerCheckUdp = NULL;
	mapPreview = NULL;
}

void NetClient::setAdminPasswd(const QString &p)
{
    adminPasswd = p;
}

void NetClient::connectToServer(QString ip, int port)
{
    tcpSocket->connectToHost(ip,port);
}

void NetClient::udpGenericStream(QDataStream & out)
{
    out.setVersion(QDataStream::Qt_4_0);
    //size
    out << (quint16)0;
    //packet count
    out << ++udpCpt;
}

void NetClient::setBlockSize(const QByteArray &block, QDataStream & out)
{
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
}

void NetClient::sendUdpWelcome()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	udpGenericStream(out);
	out << (quint16)msg_udp_welcome;
	setBlockSize(block, out);
	//qDebug() << "NetClient sendUdpWelcome" << udpSocket->localPort() << udpSocket->peerPort();
	sendUdpDatagram(block);
}

void NetClient::sendMove(int direction)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	udpGenericStream(out);
	out << (quint16)msg_move;
	out << (qint16)direction;
	setBlockSize(block, out);
	//qDebug() << "NetClient send move udp" << serverAddress << serverPort;
    sendUdpDatagram(block);
}


void NetClient::sendBomb()
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	udpGenericStream(out);
	out << (quint16)msg_bomb;
	setBlockSize(block, out);
    sendUdpDatagram(block);
}

void NetClient::sendOptKey()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    udpGenericStream(out);
    out << (quint16)msg_opt_key;
    setBlockSize(block, out);
    sendUdpDatagram(block);
}

void NetClient::sendPing()
{
	if(lastPingAck != cptPing)
		qDebug() << "Ping : timeout";
	cptPing++;
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	udpGenericStream(out);
	out << (quint16)msg_ping;
	out << cptPing;
	setBlockSize(block, out);
    sendUdpDatagram(block);
	timePing->restart();
}

void NetClient::sendVersionNumber()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_net_version;
    out << (qint16)NET_VERSION;
    setBlockSize(block, out);
    tcpSocket->write(block);
    //qDebug() << "NetClient send version number";
}

void NetClient::sendPlayerData(const QString& playerName) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << static_cast<quint16>(0);
    out << static_cast<quint16>(msg_player_data);
    out << playerName;
    setBlockSize(block, out);
    tcpSocket->write(block);
    //qDebug() << "NetClient send player data";
}

void NetClient::selectMap(int direction)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << static_cast<quint16>(0);
    out << static_cast<quint16>(msg_select_map);
    out << static_cast<qint8>(direction);
    setBlockSize(block, out);
    tcpSocket->write(block);
    //qDebug() << "NetClient select map";
}

void NetClient::kickPlayer(qint8 playerId)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << static_cast<quint16>(0);
    out << static_cast<quint16>(msg_kick_player);
    out << static_cast<qint8>(playerId);
    setBlockSize(block, out);
    tcpSocket->write(block);
}

void NetClient::stopServer()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_shutdown_server;
    setBlockSize(block, out);
    qDebug("NetClient send msg_shutdown_server");
    tcpSocket->write(block);
}

void NetClient::disconnectFromServer() {
    tcpSocket->close();
    emit sigNetClientEnd();
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
	            emit sigStatPing(e);
			}
			else
			{
			    emit sigStatPing(-1);
				qDebug() << "Ping : received out of delay";
			}
			break;
		case msg_update_map: {
				QByteArray updateBlock;
				in >> updateBlock;
				if (map!=NULL)
					map->updateMap(updateBlock);
			}
			break;
		default:
			qDebug() << "NetClient readMove discarding unkown message";
			break;
		}
	}
}

void NetClient::readTcpMsgFromServer()
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
        handleTcpMsg(in);
        blockSize = 0;
    } while(! tcpSocket->atEnd());
}

void NetClient::handleTcpMsg(QDataStream &in)
{
	qint16 msg_type;
	in >> msg_type;
	//qDebug() << "NetClient::handleMsg tcp, type = " << msg_type;
	switch(msg_type)
	{
	case msg_map:
		qDebug("NetClient map received");
		delete map;
		map = new MapClient();
		in >> *map;
		emit mapReceived(map);
		break;
	case msg_map_preview:
        qDebug("NetClient map preview received");
        delete mapPreview;
        mapPreview = new MapClient();
        in >> *mapPreview;
        emit mapPreviewReceived(mapPreview);
        break;
    case msg_map_random:
    {
        bool rand;
        in >> rand;
        qDebug("NetClient random map");
        delete mapPreview;
        mapPreview = NULL;
        emit sigMapRandom(rand);
        break;
    }
	case msg_udp_stat:
	{
	    quint16 nbErrors;
	    in >> nbErrors;
	    qDebug() << "udp stats" << nbErrors << "packets error";
	    emit sigStatPacketLoss((double)nbErrors/UDP_STATS_INTERVAL);
	    break;
	}
	case msg_is_admin:
	{
	    //qDebug() << "NetClient, msg_is_admin";
        isAdmin = true;
	    emit sigIsServerAdmin();
	    break;
	}
	case msg_max_players:
	{
        quint16 maxPl;
        in >> maxPl;
        emit sigMaxPlayersChanged((int)maxPl);
	    break;
	}
    case msg_max_wins:
    {
        quint16 maxWin;
        in >> maxWin;
        emit sigMaxWinsChanged((int)maxWin);
        break;
    }
    case msg_update_player_data: {
            qint8 playerId;
            QString playerName;

            in >> playerId;
            in >> playerName;
            if(playerName.size() > MAX_PLAYER_NAME_LENGTH) {
                qDebug() << "Error, player #" << playerId << "'s name is too long, " << playerName.size()
                        << " instead of " << MAX_PLAYER_NAME_LENGTH;
                playerName.clear();
            }
            qDebug() << "Id: " << playerId << ", name: " << playerName;
            emit sigUpdatePlayerData(playerId, playerName);
            break;
        }
    case msg_client_disconnected :
    {
        qint8 playerId;
        in >> playerId;
        qDebug() << "Client #" << playerId << "left";
        emit sigPlayerLeft(playerId);
        break;
    }
    case msg_map_winner: {
            qint8 playerId, listSize;
            bool theEnd;
            in >> playerId;
            in >> listSize;
            for(qint8 i = 0; i < listSize; i++)
            {
                qint8 id;
                qint16 score;
                in >> id >> score;
                emit sigScoreUpdate(id,score);
            }
            in >> theEnd;
            emit sigMapWinner(playerId, theEnd);
            break;
        }
    case msg_start_game:
        //qDebug() << "NetClient emit sigGameStarted";
        emit sigGameStarted();
        break;
    case msg_server_stopped:
        qDebug() << "Server was shut down";
        emit sigServerStopped();
        break;
	default:
		//trash the message
        qDebug() << "NetClient, unexpected tcp message received" << msg_type;
		in.skipRawData(blockSize - sizeof(qint16));     // blockSize contains the message type and we already popped it
		break;
	}
}

void NetClient::startGame(int styleIndex)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_start_game;
    out << (qint8)styleIndex;
    setBlockSize(block, out);
    tcpSocket->write(block);
}

void NetClient::sendAdminPasswd()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_admin_passwd;
    out << adminPasswd;
    setBlockSize(block, out);
    tcpSocket->write(block);
}

void NetClient::setMaxPlayers(int value)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_max_players;
    out << (qint16)value;
    setBlockSize(block, out);
    tcpSocket->write(block);
}

void NetClient::setMaxWins(int value)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << (quint16)msg_max_wins;
    out << (qint16)value;
    setBlockSize(block, out);
    tcpSocket->write(block);
}

void NetClient::checkUdp()
{
	if(udpCheckCount>=10)//todo configurable
	{
		timerCheckUdp->stop();
		qDebug() << "NetClient : didn't receive udp ack. Check you firewall/router";
		emit sigConnectionError();
		return;
	}
	udpCheckCount++;
	sendUdpWelcome();
}

void NetClient::sendUdpDatagram(const QByteArray &block)
{
    udpSocket->writeDatagram(block, serverAddress, serverPort);
    //if(udpCpt % 100 == 0)
    //    qDebug() << "Client, sent" << udpCpt << "udp packets";
}

void NetClient::slotTcpConnected()
{
	serverPort = tcpSocket->peerPort();
	serverAddress = tcpSocket->peerAddress();
	bool b = udpSocket->bind(tcpSocket->localPort());
	if(b)
	{
		//qDebug() << "NetClient slotTcpConnected" << serverAddress << serverPort;
	    sendVersionNumber();
	    if(!adminPasswd.isEmpty())
	        sendAdminPasswd();
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
	delete timePing;
	delete udpSocket;
	delete tcpSocket;
	delete mapPreview;
	delete map;
}

