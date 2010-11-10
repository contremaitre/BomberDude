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

#include "startUi.h"
#include "ui_main_window.h"
#include "Settings.h"
#include "GamePlay.h"
#include <QSound>

StartUi::StartUi()
{
    gamePlay = NULL;
    server = NULL;
    settings = new Settings;
    mainWindow = new Ui_MainWindow;
    mainWindow->setupUi(this);
    loadPixmaps();
    loadNetWidget();
    loadSound();
    loadIpStats();
    connect(mainWindow->serverButton,SIGNAL(clicked()),this,SLOT(startServer()));
    connect(mainWindow->isServer, SIGNAL(stateChanged(int)), this, SLOT(isServerChanged(int)));
    connect(mainWindow->sound, SIGNAL(stateChanged(int)), this, SLOT(soundChanged(int)));
    connect(mainWindow->stats_check, SIGNAL(stateChanged(int)), this, SLOT(statsCheckedChanged(int)));

}

void StartUi::loadIpStats()
{
    Qt::CheckState checked = settings->getShowIpStats() ? Qt::Checked : Qt::Unchecked;
    mainWindow->stats_check->setCheckState(checked);
    mainWindow->ping_pic->setPixmap(statusGrey);
    mainWindow->pack_loss_pic->setPixmap(statusGrey);
}

void StartUi::loadPixmaps()
{
    QSize size(mainWindow->ping_pic->width(),mainWindow->ping_pic->height());
    QPixmap grey = QPixmap("pictures/status_grey.png");
    statusGrey = grey.scaled(size);

    QPixmap yellow = QPixmap("pictures/status_yellow.png");
    statusYellow = yellow.scaled(size);

    QPixmap green = QPixmap("pictures/status_green.png");
    statusGreen = green.scaled(size);

    QPixmap red = QPixmap("pictures/status_red.png");
    statusRed = red.scaled(size);

    QSize size2(mainWindow->serverStatus->width(),mainWindow->serverStatus->height());
    QPixmap load = QPixmap("pictures/loading.png");
    loading = load.scaled(size2);
}

void StartUi::loadSound()
{
    music = NULL;
    if(!QSound::isAvailable())
    {
        qDebug() << "No Sound available";
        mainWindow->sound->setEnabled(false);
        mainWindow->sound->setCheckState(Qt::Unchecked);
        return;
    }
    Qt::CheckState checked = settings->isSound() ? Qt::Checked : Qt::Unchecked;
    mainWindow->sound->setCheckState(checked);
    toggleMusic(settings->isSound());
}

void StartUi::loadNetWidget()
{
    Qt::CheckState checked = settings->isServer() ? Qt::Checked : Qt::Unchecked;

    mainWindow->isServer->setCheckState(checked);
    mainWindow->serverIp->setPlainText(settings->getServerAddress());
    QString port;
    port.setNum(settings->getServerPort());
    mainWindow->serverPort->setPlainText(QString(port));
    updateNetWidgetState(checked == 0);
    //mainWindow->serverStatus->setPixmap(loading);
    mainWindow->serverStatus->setPixmap(NULL);
}

bool StartUi::setSettings()
{
    bool ok, checked;
    int port = mainWindow->serverPort->toPlainText().toInt(&ok);
    if(!ok)
        return false;
    checked = mainWindow->isServer->checkState() == 0 ? false : true;
    settings->setServer(checked);
    settings->setServerPort(port);
    checked = mainWindow->stats_check->checkState() == 0 ? false : true;
    settings->setShowIpStats(checked);
    settings->setServerAddress(mainWindow->serverIp->toPlainText());
    checked = mainWindow->sound->checkState() == 0 ? false : true;
    settings->setSound(checked);
    return true;
}

void StartUi::startServer()
{
    if(!setSettings())
        return;
    mainWindow->network_pref->hide();
    mainWindow->sound_pref->hide();
    if(!settings->getShowIpStats())
        mainWindow->ip_stats->hide();

    if(settings->isServer())
    {
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
        server->start("./Serverd");
    }
    gamePlay = new GamePlay(this, settings);
    connect( gamePlay, SIGNAL(connectedToServer()), this, SLOT(slotConnectedToServer()) );
    connect( gamePlay, SIGNAL(connectionError()), this, SLOT(slotConnectionError()), Qt::QueuedConnection );
    connect( gamePlay, SIGNAL(quitGame()), this, SLOT(closeGame()), Qt::QueuedConnection );
    connect( gamePlay, SIGNAL(sigStatPing(int)), this, SLOT(statPing(int)));
    connect( gamePlay, SIGNAL(sigStatPacketLoss(double)), this, SLOT(statPacketLoss(double)));
    gamePlay->launch();
}

void StartUi::updateNetWidgetState(bool en)
{
    mainWindow->serverIp->setEnabled(en);
    mainWindow->ipLabel->setEnabled(en);
    if(en)
    {
        mainWindow->serverButton->setText("Connect");
    }
    else
    {
        mainWindow->serverButton->setText("Launch");
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
            mainWindow->pack_loss_pic->setPixmap(statusGreen);
        else if(packet_loss <= 0.01)
            mainWindow->pack_loss_pic->setPixmap(statusYellow);
        else
            mainWindow->pack_loss_pic->setPixmap(statusRed);
    }
}

void StartUi::statPing(int ping)
{
    if(settings->getShowIpStats())
    {
        if(ping < 60)
            mainWindow->ping_pic->setPixmap(statusGreen);
        else if(ping < 100)
            mainWindow->ping_pic->setPixmap(statusYellow);
        else
            mainWindow->ping_pic->setPixmap(statusRed);
    }
}


void StartUi::slotConnectedToServer()
{
    //qDebug("StartUi::slotConnected");
}

void StartUi::closeGame()
{
    delete gamePlay;
    gamePlay = NULL;
    loadIpStats();
    mainWindow->network_pref->show();
    mainWindow->sound_pref->show();
    mainWindow->ip_stats->show();
}

void StartUi::slotServerLaunched()
{
    qDebug("StartUi::slotServerLaunched");
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

void StartUi::slotReadServerDebug()
{
    qDebug() << "Server : " << server->readAllStandardOutput();
}

StartUi::~StartUi()
{
    setSettings();
    if(server)
    {
        server->kill(); //todo
        delete server;
    }
    delete gamePlay;
    delete mainWindow;
    delete settings;
    delete music;
}

