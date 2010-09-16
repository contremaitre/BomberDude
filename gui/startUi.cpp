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
#include "ui_network.h"
#include "Settings.h"
#include "GamePlay.h"

StartUi::StartUi()
{
    gamePlay = NULL;
    settings = new Settings;
    mainWindow = new Ui_MainWindow;
    network = new Ui_Network;
    mainWindow->setupUi(this);
    network->setupUi(this);
    loadNetWidget();
    connect(mainWindow->startButton,SIGNAL(clicked()),this,SLOT(start()));
    connect(network->isServer, SIGNAL(stateChanged(int)), this, SLOT(isServerChanged(int)));
}

void StartUi::loadNetWidget()
{
    Qt::CheckState checked = settings->getServer() ? Qt::Checked : Qt::Unchecked;
    network->isServer->setCheckState(checked);
    network->serverIp->setPlainText(settings->getServerAddress());
    QString port;
    port.setNum(settings->getServerPort());
    network->serverPort->setPlainText(QString(port));
    setAddrFieldEnabled(checked == 0);
}

bool StartUi::setSettings()
{
    bool ok;
    int port = network->serverPort->toPlainText().toInt(&ok);
    if(!ok)
        return false;
    bool checked = network->isServer->checkState() == 0 ? false : true; 
    settings->setServer(checked);
    settings->setServerPort(port);
    settings->setServerAddress(network->serverIp->toPlainText());
    return true;
}

void StartUi::start()
{
    if(!setSettings())
        return;
    mainWindow->startButton->hide();
    gamePlay = new GamePlay(this, settings);
    connect( gamePlay, SIGNAL(connectedToServer()), this, SLOT(slotConnected()) );
    connect( gamePlay, SIGNAL(connectionError()), this, SLOT(slotConnectionError()) );
    connect( gamePlay, SIGNAL(quitGame()), this, SLOT(closeGame()), Qt::QueuedConnection );
    gamePlay->launch();
}

void StartUi::setAddrFieldEnabled(bool en)
{
    network->serverIp->setEnabled(en);
    network->ipLabel->setEnabled(en);
}

void StartUi::isServerChanged(int state)
{
    setAddrFieldEnabled(state == 0);
}

void StartUi::slotConnected()
{
    //qDebug("StartUi::slotConnected");
    //We do not (yet) handle connection errors after this
    disconnect(gamePlay, SIGNAL(connectionError()), this, SLOT(slotConnectionError()));
}

void StartUi::closeGame()
{
    delete gamePlay;
    gamePlay = NULL;
    mainWindow->startButton->show();
}

void StartUi::slotConnectionError()
{
    //qDebug("StartUi::slotConnectionError");
    closeGame();
}

StartUi::~StartUi()
{
    setSettings();
    delete gamePlay;
    delete mainWindow;
    delete settings;
}

