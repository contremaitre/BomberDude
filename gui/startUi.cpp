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

#include <QMessageBox>

#include "startUi.h"
#include "constant.h"

StartUi::StartUi(QApplication *a)
{
    gamePlay = NULL;
    server = NULL;
    qapp = a;
    settings = new Settings;
    setupUi(this);

    playerName->setMaxLength(MAX_PLAYER_NAME_LENGTH);
    loadPixmaps();
    loadNetWidget();
    loadSound();
    loadIpStats();
    loadPlayerData();
    connect(serverButton,SIGNAL(clicked()),this,SLOT(startServer()));
    connect(isServer, SIGNAL(stateChanged(int)), this, SLOT(isServerChanged(int)));
    connect(sound, SIGNAL(stateChanged(int)), this, SLOT(soundChanged(int)));
    connect(stats_check, SIGNAL(stateChanged(int)), this, SLOT(statsCheckedChanged(int)));
    connect(randomMapCheck, SIGNAL(stateChanged(int)), this, SLOT(randomMapCheckedChanged(int)));
    connect(startGameButton,SIGNAL(clicked()),this,SLOT(slotStartGame()));
    connect(cancelGameButton,SIGNAL(clicked()),this,SLOT(slotStopGame()));
    connect(mapRightButton, SIGNAL(clicked()), this, SLOT(slotMapRightButton()));
    connect(mapLeftButton, SIGNAL(clicked()), this, SLOT(slotMapLeftButton()));
    connect(stopGame, SIGNAL(clicked()), this, SLOT(closeGame()));
}

void StartUi::loadIpStats()
{
    Qt::CheckState checked = settings->getShowIpStats() ? Qt::Checked : Qt::Unchecked;
    stats_check->setCheckState(checked);
    ping_pic->setPixmap(statusGrey);
    pack_loss_pic->setPixmap(statusGrey);
}

void StartUi::loadPixmaps()
{
    QSize size(ping_pic->width(),ping_pic->height());
    QPixmap grey = QPixmap("pictures/status_grey.png");
    statusGrey = grey.scaled(size);

    QPixmap yellow = QPixmap("pictures/status_yellow.png");
    statusYellow = yellow.scaled(size);

    QPixmap green = QPixmap("pictures/status_green.png");
    statusGreen = green.scaled(size);

    QPixmap red = QPixmap("pictures/status_red.png");
    statusRed = red.scaled(size);

    QSize size2(serverStatus->width(),serverStatus->height());
    QPixmap load = QPixmap("pictures/loading.png");
    loading = load.scaled(size2);
}

void StartUi::loadSound()
{
    music = NULL;
    if(!QSound::isAvailable())
    {
        qDebug() << "No Sound available";
        sound->setEnabled(false);
        sound->setCheckState(Qt::Unchecked);
        return;
    }
    Qt::CheckState checked = settings->isSound() ? Qt::Checked : Qt::Unchecked;
    sound->setCheckState(checked);
    toggleMusic(settings->isSound());
}

void StartUi::loadNetWidget()
{
    Qt::CheckState checked = settings->isServer() ? Qt::Checked : Qt::Unchecked;

    isServer->setCheckState(checked);
    serverIp->setPlainText(settings->getServerAddress());
    QString port;
    port.setNum(settings->getServerPort());
    serverPort->setPlainText(QString(port));
    updateNetWidgetState(checked == 0);
    //serverStatus->setPixmap(loading);
    serverStatus->setPixmap(NULL);
    maxPlayersBox->setValue(0);
    adminWidget->setEnabled(false);
    randomMapCheck->setCheckState(Qt::Unchecked);
    mapRightButton->setEnabled(true);
    mapLeftButton->setEnabled(true);
    checkDebugMode->setChecked(settings->isDebugMode());
}

void StartUi::loadPlayerData() {
    playerName->setText(settings->getPlayerName());
}

bool StartUi::setSettings()
{
    bool ok, checked;
    int port = serverPort->toPlainText().toInt(&ok);
    if(!ok)
        return false;
    checked = isServer->checkState() == 0 ? false : true;
    settings->setServer(checked);
    settings->setServerPort(port);
    checked = stats_check->checkState() == 0 ? false : true;
    settings->setShowIpStats(checked);
    settings->setServerAddress(serverIp->toPlainText());
    checked = sound->checkState() == 0 ? false : true;
    settings->setSound(checked);
    settings->setPlayerName(playerName->text());
    settings->setDebugMode(checkDebugMode->isChecked());
    return true;
}

void StartUi::startServer()
{
    if(!setSettings())
        return;
    network_pref->setEnabled(false);
    sound_pref->setEnabled(false);
    if(!settings->getShowIpStats())
        ip_stats->hide();

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
		serverCmdLine += serverPort->toPlainText();
		serverCmdLine += " --admin-password " + adminPassword;
        if(checkDebugMode->isChecked())
            serverCmdLine += " --debug-mode";
        serverCmdLine += " --started-from-gui";

        server->start(serverCmdLine);
    }
    else
    {
        adminPassword = password->toPlainText();
    }
    gamePlay = new GamePlay(this, settings, previewGraphicsView);
    NetClient *netclient = gamePlay->getNetClient();

    connect( gamePlay, SIGNAL(quitGame()), this, SLOT(closeGame()), Qt::QueuedConnection );
    connect( gamePlay, SIGNAL(sigTimeUpdated(int)), gameClock, SLOT(display(int)));
    connect( gamePlay, SIGNAL(sigNewPlayerGraphic(int,const QPixmap &)), this, SLOT(slotNewPlayerGraphic(int,const QPixmap &)));
    connect( netclient, SIGNAL(sigConnected()), this, SLOT(slotConnectedToServer()));
    connect( netclient, SIGNAL(sigConnectionError()), this, SLOT(slotConnectionError()), Qt::QueuedConnection);
    connect( netclient, SIGNAL(sigStatPing(int)), this, SLOT(statPing(int)));
    connect( netclient, SIGNAL(sigStatPacketLoss(double)), this, SLOT(statPacketLoss(double)));
    connect( netclient, SIGNAL(sigIsServerAdmin()), this, SLOT(slotIsServerAdmin()));
    connect( netclient, SIGNAL(sigMaxPlayersChanged(int)), this, SLOT(slotMaxPlayersChanged(int)));
    connect( netclient, SIGNAL(sigUpdatePlayerData(qint32,QString)), this, SLOT(slotUpdatePlayerData(qint32,QString)));
    connect( netclient, SIGNAL(sigPlayerLeft(qint32)), this, SLOT(slotPlayerLeft(qint32)));
    connect( netclient, SIGNAL(sigMapRandom()), this, SLOT(slotMapRandom()));
    connect( netclient, SIGNAL(sigGameStarted()), this, SLOT(slotGameStarted()));
    connect( netclient, SIGNAL(mapPreviewReceived(MapClient*)),this,SLOT(slotMapPreviewReceived(MapClient*)));
    //connect( netclient, SIGNAL(sigGameQuit()), this, SLOT(slotGameQuit()));

    // must be queued otherwise NetClient instance is deleted before finishing its processing
    connect( netclient, SIGNAL(sigServerStopped()), this, SLOT(slotServerStopped()), Qt::QueuedConnection);

    // if the server is remote, try to connect immediately (otherwise server's output must be checked)
    if(!settings->isServer())
        gamePlay->cliConnect(adminPassword);
}

void StartUi::updateNetWidgetState(bool en)
{
    serverIp->setEnabled(en);
    ipLabel->setEnabled(en);
    passwordLabel->setEnabled(en);
    password->setEnabled(en);
    if(en)
    {
        serverButton->setText("Connect");
    }
    else
    {
        serverButton->setText("Launch");
    }
}

void StartUi::isServerChanged(int state)
{
    updateNetWidgetState(state == 0);
}

void StartUi::statsCheckedChanged(int state)
{
    qDebug("statsCheckedChanged");
}

void StartUi::soundChanged(int state)
{
    toggleMusic(state != 0);
}

void StartUi::maxPlayersValueChanged(int value)
{
    qDebug() << "maxPlayersValueChanged" << value;
    if(gamePlay)
        gamePlay->getNetClient()->setMaxPlayers(value);
}

void StartUi::toggleMusic(bool on)
{
    delete music;
    music = NULL;
    qDebug() << "music" << on;
    if(on)
    {
        music = new QSound("sounds/music.wav",this);
        music->setLoops(-1);
        music->play();
    }
}

void StartUi::statPacketLoss(double packet_loss)
{
    if(settings->getShowIpStats())
    {
        if(packet_loss == 0)
            pack_loss_pic->setPixmap(statusGreen);
        else if(packet_loss <= 0.01)
            pack_loss_pic->setPixmap(statusYellow);
        else
            pack_loss_pic->setPixmap(statusRed);
    }
}

void StartUi::statPing(int ping)
{
    if(settings->getShowIpStats())
    {
        if(ping < 60)
            ping_pic->setPixmap(statusGreen);
        else if(ping < 100)
            ping_pic->setPixmap(statusYellow);
        else
            ping_pic->setPixmap(statusRed);
    }
}

void StartUi::slotConnectedToServer()
{
    qDebug("StartUi::slotConnected");
    serverStatus->setPixmap(loading);
    if(gamePlay)
        gamePlay->getNetClient()->sendPlayerData(playerName->text());
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
    loadIpStats();
    loadNetWidget();
    playersList->clear();
    network_pref->show();
    sound_pref->show();
    ip_stats->show();
    adminWidget->show();
    player_data->show();
    previewGraphicsView->show();
    network_pref->setEnabled(true);
    sound_pref->setEnabled(true);
    stopGame->setEnabled(false);
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

void StartUi::slotMapRandom()
{
    randomMapCheck->setCheckState(Qt::Checked);
}

void StartUi::slotStartGame()
{
    int styleIndex = mapOptionCBox->currentIndex();
    //qDebug() << "startui start game, style =" << styleIndex;
    if(gamePlay)
        gamePlay->getNetClient()->startGame(styleIndex-1);
}

void StartUi::slotStopGame()
{
    //qDebug() << "startui stop game";
    if(gamePlay) {
        if(gamePlay->getNetClient()->getIsAdmin())
            gamePlay->getNetClient()->stopServer();
        else
            gamePlay->getNetClient()->quitGame();
    }
}

void StartUi::slotGameStarted()
{
    qDebug() << "StartUi game started";
    network_pref->hide();
    sound_pref->hide();
    adminWidget->hide();
    player_data->hide();
    previewGraphicsView->hide();
    stopGame->setEnabled(true);
}

void StartUi::slotMapPreviewReceived(MapClient *map)
{
    const QList<mapStyle> *styles = map->getStyles();
    mapOptionCBox->clear();
    mapOptionCBox->addItem ( "no style" );
    foreach(mapStyle s, *styles)
        mapOptionCBox->addItem ( s.name );
}

void StartUi::randomMapCheckedChanged(int state)
{
    if(!gamePlay || !gamePlay->getNetClient())
        return;
    if(state == 0)
    {
        //non random map
        mapRightButton->setEnabled(true);
        mapLeftButton->setEnabled(true);
        gamePlay->getNetClient()->selectMap(2);
    }
    else
    {
        //random map
        mapRightButton->setEnabled(false);
        mapLeftButton->setEnabled(false);
        gamePlay->getNetClient()->selectMap(0);
    }
}

void StartUi::slotMapLeftButton()
{
    gamePlay->getNetClient()->selectMap(-1);
}

void StartUi::slotMapRightButton()
{
    gamePlay->getNetClient()->selectMap(1);
}

void StartUi::slotIsServerAdmin()
{
    maxPlayersBox->setMinimum(1);
    adminWidget->setEnabled(true);
    connect(maxPlayersBox, SIGNAL(valueChanged(int)), this, SLOT(maxPlayersValueChanged(int)));
}

void StartUi::slotMaxPlayersChanged(int maxPlayers)
{
    maxPlayersBox->setValue(maxPlayers);
}

void StartUi::slotUpdatePlayerData(qint32 playerId, QString playerName) {
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(playerId));
    playersList->setItem(playerId, 0, newItem);
    newItem = new QTableWidgetItem(playerName);
    playersList->setItem(playerId, 1, newItem);
}

void StartUi::slotPlayerLeft(qint32 playerId)
{
    delete playersList->takeItem(playerId,0);
    delete playersList->takeItem(playerId,1);
}

void StartUi::slotNewPlayerGraphic(int player, const QPixmap &pix)
{
    qDebug() << "Startui new player graphic" << player;
    QLabel *label = new QLabel;
    label->setPixmap(pix);
    playerListLayout->addWidget(label,player,0);

    QLabel *label2 = new QLabel(playersList->item(player,1)->text());
    playerListLayout->addWidget(label2,player,1);
    labelsPlayerList.push_back(label);
    labelsPlayerList.push_back(label2);
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
    setSettings();
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
}
