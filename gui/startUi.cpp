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

#include <QMessageBox>

#include "startUi.h"
#include "Settings.h"
#include "GamePlay.h"
#include "GameFrame.h"
#include "MenuFrame.h"

StartUi::StartUi(QApplication *a)
{
    gamePlay = NULL;
    server = NULL;
    menuFrame = NULL;
    gameFrame = NULL;
    qapp = a;
    settings = new Settings;
    setupUi(this);
    loadMenuFrame();
    music = new QSound("sounds/music.wav",this);
}

void StartUi::loadMenuFrame()
{
    if( menuFrame )
        delete menuFrame;
    menuFrame = new MenuFrame(settings, &ipStats, music);
    connect(menuFrame->ui.maxPlayersBox, SIGNAL(valueChanged(int)), this, SLOT(slotMaxPlayersValueChanged(int)));
    connect(menuFrame->ui.serverButton,SIGNAL(clicked()),this,SLOT(slotStartServer()));
    connect(menuFrame->ui.mapRightButton, SIGNAL(clicked()), this, SLOT(slotMapRightButton()));
    connect(menuFrame->ui.mapLeftButton, SIGNAL(clicked()), this, SLOT(slotMapLeftButton()));
    connect(menuFrame->ui.randomMapCheck, SIGNAL(stateChanged(int)), this, SLOT(randomMapCheckedChanged(int)));
    connect(menuFrame, SIGNAL(sigStart(int)),this,SLOT(slotStartGame(int)));
    connect(menuFrame, SIGNAL(sigDisconnect()),this,SLOT(slotDisconnectGame()));
    connect(menuFrame, SIGNAL(sigKickPlayer(int)), this, SLOT(slotKickPlayer(int)));
    gridLayout->addWidget(menuFrame);
}

void StartUi::loadGameFrame()
{
    if( gameFrame )
        return;
    gameFrame = new GameFrame( &ipStats );
    connect( gamePlay, SIGNAL(sigTimeUpdated(int)), gameFrame->ui.gameClock, SLOT(display(int)));
    connect( gameFrame->ui.disconnectGameButton, SIGNAL(clicked()), this, SLOT(slotDisconnectGame()));
    gridLayout->addWidget(gameFrame);
}

void StartUi::slotStartServer()
{
    /* todo GameFrame
    if(!settings->getShowIpStats())
        ip_stats->hide();
*/
    menuFrame->setSettings();

    if(settings->isServer())
    {
        qsrand(QDateTime::currentDateTime().toTime_t());
        adminPassword.setNum(qrand ());
        if(server)
        {
            server->kill();
            delete server;
        }
        server = new QProcess(this);
        connect(server, SIGNAL(started()), this, SLOT(slotServerLaunched()));
        connect(server, SIGNAL(error(QProcess::ProcessError)), this, SLOT(slotServerLaunchedError(QProcess::ProcessError)));
        //read debug ouputs
        connect(server,SIGNAL(readyReadStandardOutput()),this,SLOT(slotReadServerDebug()));
        server->setReadChannelMode(QProcess::MergedChannels);

		QString serverCmdLine("./Serverd");
		serverCmdLine += " --port ";
		serverCmdLine += settings->getServerPort();
		serverCmdLine += " --admin-password " + adminPassword;
        if(settings->isDebugMode())
            serverCmdLine += " --debug-mode";
        serverCmdLine += " --started-from-gui";

        server->start(serverCmdLine);
    }
    else
    {
        adminPassword = menuFrame->getAdminPassword();
    }
    gamePlay = new GamePlay(this, settings, menuFrame->getGraphicPreview());
    NetClient *netclient = gamePlay->getNetClient();

    connect( gamePlay, SIGNAL(quitGame()), this, SLOT(closeGame()), Qt::QueuedConnection );
    connect( gamePlay, SIGNAL(sigNewPlayerGraphic(int,const QPixmap &)), this, SLOT(slotNewPlayerGraphic(int,const QPixmap &)));
    menuFrame->setNetClient(netclient);
    connect( netclient, SIGNAL(sigConnected()), this, SLOT(slotConnectedToServer()));
    connect( netclient, SIGNAL(sigConnectionError()), this, SLOT(slotConnectionError()), Qt::QueuedConnection);
    connect( netclient, SIGNAL(sigStatPing(int)), this, SLOT(statPing(int)));
    connect( netclient, SIGNAL(sigStatPacketLoss(double)), this, SLOT(statPacketLoss(double)));
    connect( netclient, SIGNAL(sigGameStarted()), this, SLOT(slotGameStarted()));
    connect( netclient, SIGNAL(sigNetClientEnd()), this, SLOT(closeGame()));

    // must be queued otherwise NetClient instance is deleted before finishing its processing
    connect( netclient, SIGNAL(sigServerStopped()), this, SLOT(slotServerStopped()), Qt::QueuedConnection);

    // if the server is remote, try to connect immediately (otherwise server's output must be checked)
    if(!settings->isServer())
        gamePlay->cliConnect(adminPassword);
}

void StartUi::slotMaxPlayersValueChanged(int value)
{
    if(gamePlay)
        gamePlay->getNetClient()->setMaxPlayers(value);
}

void StartUi::statPacketLoss(double packet_loss)
{
    if (packet_loss == 0)
        ipStats.setPacketLostState(STATUS_OK);
    else if (packet_loss <= 0.01)
        ipStats.setPacketLostState(STATUS_WARN);
    else
        ipStats.setPacketLostState(STATUS_BAD);
}

void StartUi::statPing(int ping)
{
    if (ping < 60)
        ipStats.setPingState(STATUS_OK);
    else if (ping < 100)
        ipStats.setPingState(STATUS_WARN);
    else
        ipStats.setPingState(STATUS_BAD);
}

void StartUi::slotConnectedToServer()
{
    qDebug("StartUi::slotConnected");
    if(gamePlay)
        gamePlay->getNetClient()->sendPlayerData(menuFrame->getPlayerName());
}

void StartUi::closeGame()
{
    qDebug("StartUi closeGame");
    if(gamePlay)
    {
        if(server)
        {
            gamePlay->getNetClient()->stopServer();
            /* waitForFinished is blocking, so let the event loop run before it */
            qapp->processEvents();
            server->waitForFinished();
            delete server;
            server = NULL;
        }
        delete gamePlay;
        gamePlay = NULL;
    }
    while(!labelsPlayerList.empty())
        delete labelsPlayerList.takeFirst();
    delete gameFrame;
    gameFrame = NULL;
    loadMenuFrame();
}

void StartUi::slotServerLaunched()
{
}

void StartUi::slotServerLaunchedError(QProcess::ProcessError error)
{
    qDebug() << "StartUi::slotServerLaunchedError" << error;
}

void StartUi::slotConnectionError()
{
    //qDebug("StartUi::slotConnectionError");
    closeGame();
}

void StartUi::slotStartGame(int styleIndex)
{
    //qDebug() << "startui start game, style =" << styleIndex;
    if(gamePlay)
        gamePlay->getNetClient()->startGame(styleIndex-1);
}

void StartUi::slotKickPlayer(int playerId)
{
    gamePlay->getNetClient()->kickPlayer(playerId);
}

void StartUi::slotDisconnectGame()
{
    if(gamePlay) {
        if(gamePlay->getNetClient()->getIsAdmin())
            gamePlay->getNetClient()->stopServer();
        else
            gamePlay->getNetClient()->disconnectFromServer();
    }
}

void StartUi::slotGameStarted()
{
    qDebug() << "StartUi game started";
    delete menuFrame;
    menuFrame = NULL;
    loadGameFrame();
}

void StartUi::randomMapCheckedChanged(int state)
{
    if(!gamePlay || !gamePlay->getNetClient())
        return;
    if(state == 0)
        gamePlay->getNetClient()->selectMap(2);
    else
        gamePlay->getNetClient()->selectMap(0);
}

void StartUi::slotMapLeftButton()
{
    gamePlay->getNetClient()->selectMap(-1);
}

void StartUi::slotMapRightButton()
{
    gamePlay->getNetClient()->selectMap(1);
}

void StartUi::slotNewPlayerGraphic(int player, const QPixmap &pix)
{
    /*TODO Move this to a separate widget
    qDebug() << "Startui new player graphic" << player;
    QLabel *label = new QLabel;
    label->setPixmap(pix);
    playerListLayout->addWidget(label,player,0);

    QLabel *label2 = new QLabel(playersList->item(player,1)->text());
    playerListLayout->addWidget(label2,player,1);
    labelsPlayerList.push_back(label);
    labelsPlayerList.push_back(label2);*/
}

void StartUi::slotReadServerDebug()
{
    QByteArray array = server->readAllStandardOutput();
    array.chop(1); //we don't want the last end of line

    bool multiline = array.contains('\n');
    if(multiline)
        qDebug() << "\n** Server debug **\n" << array << "\n** Server debug end **\n";
    else
        qDebug() << "Server debug:"<< array;

    if(settings->isServer() && array.contains(SIGNAL_SERVER_LISTENING)) {
        gamePlay->cliConnect(adminPassword);
        qDebug() << "Server is listening: connecting";
    }
}

void StartUi::slotServerStopped()
{
    QMessageBox::warning(this, QString("Server stopped"),QString("The server was shut down, either by admin command or because the connection with the admin GUI was lost"));
}

StartUi::~StartUi()
{
    //setSettings(); TODO
    if(server)
    {
        if(gamePlay)
        {
            gamePlay->getNetClient()->stopServer();
            /* waitForFinished is blocking, so let the event loop run before it */
            qapp->processEvents();
        }
        server->waitForFinished();
        delete server;
    }
    delete gamePlay;
    delete settings;
    delete music;
    delete gameFrame;
    delete menuFrame;
}
