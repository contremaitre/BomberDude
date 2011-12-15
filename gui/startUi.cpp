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
#include "MenuTabFrame.h"
#include "InterGameFrame.h"

StartUi::StartUi(QApplication *a)
{
    gamePlay = NULL;
    server = NULL;
    menuTabFrame = NULL;
    gameFrame = NULL;
    interGameFrame = NULL;
    styleIndex = -1;
    qapp = a;
    settings = new Settings;
    setupUi(this);
    loadMenuTabFrame();
    //music = new QSound("sounds/music.wav",this);
    music = NULL;
}

void StartUi::loadMenuTabFrame()
{
    if( menuTabFrame )
        delete menuTabFrame;
    menuTabFrame = new MenuTabFrame(settings, &ipStats, &playerListWidget, music);
    connect(menuTabFrame->MainUi.maxPlayersBox, SIGNAL(valueChanged(int)), this, SLOT(slotMaxPlayersValueChanged(int)));
    connect(menuTabFrame->MainUi.maxWinsBox, SIGNAL(valueChanged(int)), this, SLOT(slotMaxWinsValueChanged(int)));
    connect(menuTabFrame->MainUi.serverButton,SIGNAL(clicked()),this,SLOT(slotStartServer()));
    connect(menuTabFrame->MainUi.mapRightButton, SIGNAL(clicked()), this, SLOT(slotMapRightButton()));
    connect(menuTabFrame->MainUi.mapLeftButton, SIGNAL(clicked()), this, SLOT(slotMapLeftButton()));
    connect(menuTabFrame->MainUi.randomMapCheck, SIGNAL(stateChanged(int)), this, SLOT(randomMapCheckedChanged(int)));
    connect(menuTabFrame->MainUi.addLocalPlayerButton, SIGNAL(clicked()), this, SLOT(slotAddLocalPlayer()));
    connect(menuTabFrame, SIGNAL(sigStart(int)),this,SLOT(slotStartGame(int)));
    connect(menuTabFrame, SIGNAL(sigDisconnect()),this,SLOT(slotDisconnectGame()));
    connect(menuTabFrame, SIGNAL(sigKickPlayer(qint8)), this, SLOT(slotKickPlayer(qint8)));
    gridLayout->addWidget(menuTabFrame);
}

void StartUi::loagInterGameFrame()
{
    delete interGameFrame;
    interGameFrame = new InterGameFrame( &playerListWidget, gamePlay->getNetClient()->getIsAdmin() );
    connect(interGameFrame->ui.nextRoundButton,SIGNAL(clicked()),this,SLOT(slotNextRound()));
    gridLayout->addWidget(interGameFrame);
}

void StartUi::loadGameFrame()
{
    if( gameFrame )
        return;
    gameFrame = new GameFrame( &ipStats, &playerListWidget );
    connect( gamePlay, SIGNAL(sigTimeUpdated(int)), gameFrame->ui.gameClock, SLOT(display(int)));
    connect( gameFrame->ui.disconnectGameButton, SIGNAL(clicked()), this, SLOT(slotDisconnectGame()));
    gridLayout->addWidget(gameFrame);
}

void StartUi::slotLoadInterGame()
{
    delete menuTabFrame;
    menuTabFrame = NULL;
    delete gameFrame;
    gameFrame = NULL;
    loagInterGameFrame();
}

void StartUi::slotStartServer()
{
    /* todo GameFrame
    if(!settings->getShowIpStats())
        ip_stats->hide();
*/
    menuTabFrame->setSettings();

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
        adminPassword = menuTabFrame->getAdminPassword();
    }
    gamePlay = new GamePlay(settings, menuTabFrame->getGraphicPreview(), menuTabFrame->getPlayerName());
    NetClient *netclient = gamePlay->getNetClient();

    connect( gamePlay, SIGNAL(quitGame()), this, SLOT(closeGame()), Qt::QueuedConnection );

    /* Connect some signals to the ourselves */
    connect( netclient, SIGNAL(sigConnectionError()), this, SLOT(slotConnectionError()), Qt::QueuedConnection);
    connect( netclient, SIGNAL(sigGameStarted()), this, SLOT(slotGameStarted()));
    connect( netclient, SIGNAL(sigNetClientEnd()), this, SLOT(closeGame()));
    connect( netclient, SIGNAL(sigMapWinner(qint8,bool)), this, SLOT(slotEndRound(qint8,bool)));

    /* Connect some signals to the IP stats widget */
    connect( netclient, SIGNAL(sigStatPing(int)), &ipStats, SLOT(slotStatPing(int)));
    connect( netclient, SIGNAL(sigStatPacketLoss(double)), &ipStats, SLOT(slotStatPacketLoss(double)));

    /* Connect some signals to the frame menu */
    connect( netclient, SIGNAL(sigConnected()), menuTabFrame, SLOT(slotConnectedToServer()));
    connect( netclient, SIGNAL(sigIsServerAdmin()), menuTabFrame, SLOT(slotIsServerAdmin()));
    connect( netclient, SIGNAL(sigUpdatePlayerData(qint8,QString)), menuTabFrame, SLOT(slotUpdatePlayerData(qint8,QString)));
    connect( netclient, SIGNAL(sigMaxPlayersChanged(int)), menuTabFrame, SLOT(slotMaxPlayersValueChanged(int)));
    connect( netclient, SIGNAL(sigMaxWinsChanged(int)), menuTabFrame, SLOT(slotMaxWinsValueChanged(int)));
    connect( netclient, SIGNAL(sigMapRandom(bool)), menuTabFrame, SLOT(slotMapRandom(bool)));
    connect( netclient, SIGNAL(mapPreviewReceived(MapClient*)), menuTabFrame,SLOT(slotMapPreviewReceived(MapClient*)));
    connect( netclient, SIGNAL(sigPlayerLeft(qint8)), menuTabFrame, SLOT(slotPlayerLeft(qint8)));

    /* Connect some signals to the player list */
    connect( netclient, SIGNAL(sigUpdatePlayerData(qint8, QString)), &playerListWidget, SLOT(slotAddPlayer(qint8, QString)));
    connect( gamePlay, SIGNAL(sigNewPlayerGraphic(qint8,const QPixmap &)), &playerListWidget, SLOT(slotNewPlayerGraphic(qint8,const QPixmap &)));
    connect( netclient, SIGNAL(sigPlayerLeft(qint8)), &playerListWidget, SLOT(slotRemovePlayer(qint8)));
    connect( netclient, SIGNAL(sigScoreUpdate(qint8, qint16)), &playerListWidget, SLOT(slotUpdatePlayerScore(qint8, qint16)));
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

void StartUi::slotMaxWinsValueChanged(int value)
{
    qDebug() << "StartUi::slotMaxWinsValueChanged" << value;
    if(gamePlay)
        gamePlay->getNetClient()->setMaxWins(value);
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
    playerListWidget.clear();
    delete gameFrame;
    gameFrame = NULL;
    delete interGameFrame;
    interGameFrame = NULL;
    loadMenuTabFrame();
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

void StartUi::slotStartGame(int style)
{
    //qDebug() << "startui start game, style =" << styleIndex;
    styleIndex = style;
    if(gamePlay)
        gamePlay->getNetClient()->startGame(styleIndex-1);
}

void StartUi::slotNextRound()
{
    gamePlay->getNetClient()->startGame(styleIndex-1);
}

void StartUi::slotKickPlayer(qint8 playerId)
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
    delete menuTabFrame;
    menuTabFrame = NULL;
    delete interGameFrame;
    interGameFrame = NULL;
    loadGameFrame();
    gamePlay->gameStarted(gameFrame->ui.gameGraphicView);
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

void StartUi::slotAddLocalPlayer()
{
    gamePlay->addPlayer(menuTabFrame->getPlayerName());
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

void StartUi::slotEndRound(qint8, bool end)
{
    if(!end)
        QTimer::singleShot(1000*5, this, SLOT(slotLoadInterGame()));
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
    delete menuTabFrame;
    delete interGameFrame;
}
