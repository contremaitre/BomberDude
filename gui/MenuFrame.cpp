#include <QtGui>
#include <QSound>

#include "MenuFrame.h"
#include "constant.h"
#include "Settings.h"
#include "IpStats.h"
#include "PlayerListWidget.h"
#include "NetClient.h"

MenuFrame::MenuFrame(Settings *settings, IpStats *ipStats, PlayerListWidget *playerListWidget, QSound *music, QFrame *parent)
    : QFrame(parent), settings(settings), ipStats(ipStats), playerListWidget(playerListWidget), music(music)
{
    ui.setupUi(this);
    ui.playerName->setMaxLength(MAX_PLAYER_NAME_LENGTH);
    loadNetWidget();
    loadSound();
    loadPlayerData();
    connect(ui.isServer, SIGNAL(stateChanged(int)), this, SLOT(isServerChanged(int)));
    connect(ui.sound, SIGNAL(stateChanged(int)), this, SLOT(soundChanged(int)));
    connect(ui.startGameButton,SIGNAL(clicked()),this,SLOT(slotStartButton()));
    connect(ui.disconnectButton,SIGNAL(clicked()),this,SLOT(slotDisconnectButton()));
    connect(ui.serverButton,SIGNAL(clicked()),this,SLOT(slotStartServer()));
    connect(ui.kickButton,SIGNAL(clicked()),this,SLOT(slotKickButton()));

    //connect(ui.stopGame, SIGNAL(clicked()), this, SLOT(closeGame())); TODO

    ipStats->setParent(ui.IpStatHolder);
    playerListWidget->setParent(ui.playerListWidgetHolder);
    QSize size2(ui.serverStatus->width(), ui.serverStatus->height());
    QPixmap load = QPixmap("pictures/loading.png");
    loading = load.scaled(size2);
}

void MenuFrame::loadSound()
{
    if(!QSound::isAvailable())
    {
        qDebug() << "No Sound available";
        ui.sound->setEnabled(false);
        ui.sound->setCheckState(Qt::Unchecked);
        return;
    }
    Qt::CheckState checked = settings->isSound() ? Qt::Checked : Qt::Unchecked;
    ui.sound->setCheckState(checked);
    toggleMusic(settings->isSound());
}

void MenuFrame::toggleMusic(bool on)
{
    qDebug() << "music" << on;
    if(on)
    {
        music->setLoops(-1);
        music->play();
    }
    else
        music->stop();
}

void MenuFrame::loadNetWidget()
{
    Qt::CheckState checked = settings->isServer() ? Qt::Checked : Qt::Unchecked;

    ui.isServer->setCheckState(checked);
    ui.serverIp->setPlainText(settings->getServerAddress());
    QString port;
    port.setNum(settings->getServerPort());
    ui.serverPort->setPlainText(QString(port));
    updateNetWidgetState(checked == 0);
    ui.serverStatus->setPixmap(NULL);
    ui.maxPlayersBox->setValue(0);
    ui.adminWidget->setEnabled(false);
    ui.randomMapCheck->setCheckState(Qt::Unchecked);
    ui.mapRightButton->setEnabled(true);
    ui.mapLeftButton->setEnabled(true);
    ui.checkDebugMode->setChecked(settings->isDebugMode());
}

void MenuFrame::loadPlayerData() {
    ui.playerName->setText(settings->getPlayerName());
}

bool MenuFrame::setSettings()
{
    bool ok, checked;
    int port = ui.serverPort->toPlainText().toInt(&ok);
    if(!ok)
        return false;
    checked = ui.isServer->checkState() == 0 ? false : true;
    settings->setServer(checked);
    settings->setServerPort(port);
    settings->setServerAddress(ui.serverIp->toPlainText());
    checked = ui.sound->checkState() == 0 ? false : true;
    settings->setSound(checked);
    settings->setPlayerName(ui.playerName->text());
    settings->setDebugMode(ui.checkDebugMode->isChecked());
    return true;
}

void MenuFrame::updateNetWidgetState(bool en)
{
    ui.serverIp->setEnabled(en);
    ui.ipLabel->setEnabled(en);
    ui.passwordLabel->setEnabled(en);
    ui.password->setEnabled(en);
    if(en)
    {
        ui.serverButton->setText("Connect");
    }
    else
    {
        ui.serverButton->setText("Launch");
    }
}

void MenuFrame::slotConnectedToServer()
{
    ui.serverStatus->setPixmap(loading);
}

void MenuFrame::slotStartServer()
{
    ui.network_pref->setEnabled(false);
    ui.disconnectButton->setEnabled(true);
}

void MenuFrame::slotIsServerAdmin()
{
    ui.maxPlayersBox->setMinimum(1);
    ui.adminWidget->setEnabled(true);
}

void MenuFrame::slotMaxPlayersValueChanged(int maxPlayers)
{
    ui.maxPlayersBox->setValue(maxPlayers);
}

void MenuFrame::slotPlayerLeft(qint32 playerId)
{
    delete ui.playersList->takeItem(playerId,0);
    delete ui.playersList->takeItem(playerId,1);
}

void MenuFrame::slotKickButton()
{
    QList<QTableWidgetItem *> list = ui.playersList->selectedItems ();
    if( !list.empty())
    {
        QTableWidgetItem *item = list.first();
        int num = item->text().toInt();
        emit sigKickPlayer(num);
    }
}

void MenuFrame::slotMapRandom(bool rand)
{
    if(rand)
    {
        ui.randomMapCheck->setCheckState(Qt::Checked);
        ui.mapRightButton->setEnabled(false);
        ui.mapLeftButton->setEnabled(false);
    }
    else
    {
        ui.randomMapCheck->setCheckState(Qt::Unchecked);
        ui.mapRightButton->setEnabled(true);
        ui.mapLeftButton->setEnabled(true);
    }

}

void MenuFrame::slotDisconnectButton()
{
    emit sigDisconnect();
}

void MenuFrame::slotStartButton()
{
    emit sigStart(ui.mapOptionCBox->currentIndex());
}

void MenuFrame::slotUpdatePlayerData(qint32 playerId, QString playerName) {
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(playerId));
    ui.playersList->setItem(playerId, 0, newItem);
    newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    newItem = new QTableWidgetItem(playerName);
    newItem->setFlags(Qt::ItemIsEnabled);
    ui.playersList->setItem(playerId, 1, newItem);
}

void MenuFrame::slotMapPreviewReceived(MapClient *map)
{
    const QList<mapStyle> *styles = map->getStyles();
    ui.mapOptionCBox->clear();
    ui.mapOptionCBox->addItem ( "no style" );
    foreach(mapStyle s, *styles)
        ui.mapOptionCBox->addItem ( s.name );
}

void MenuFrame::isServerChanged(int state)
{
    updateNetWidgetState(state == 0);
}

void MenuFrame::soundChanged(int state)
{
    toggleMusic(state != 0);
}

void MenuFrame::setNetClient(NetClient *netclient)
{
    connect( netclient, SIGNAL(sigConnected()), this, SLOT(slotConnectedToServer()));
    connect( netclient, SIGNAL(sigIsServerAdmin()), this, SLOT(slotIsServerAdmin()));
    connect( netclient, SIGNAL(sigUpdatePlayerData(qint32,QString)), this, SLOT(slotUpdatePlayerData(qint32,QString)));
    connect( netclient, SIGNAL(sigMaxPlayersChanged(int)), this, SLOT(slotMaxPlayersValueChanged(int)));
    connect( netclient, SIGNAL(sigMapRandom(bool)), this, SLOT(slotMapRandom(bool)));
    connect( netclient, SIGNAL(mapPreviewReceived(MapClient*)),this,SLOT(slotMapPreviewReceived(MapClient*)));
    connect( netclient, SIGNAL(sigPlayerLeft(qint32)), this, SLOT(slotPlayerLeft(qint32)));
}

const QString MenuFrame::getPlayerName() const
{
    return ui.playerName->text();
}

QString MenuFrame::getAdminPassword() const
{
    return ui.password->toPlainText();
}

QGraphicsView *MenuFrame::getGraphicPreview() const
{
    return ui.previewGraphicsView;
}

MenuFrame::~MenuFrame()
{
    //todo destroy player list
    ipStats->setParent(NULL);
    playerListWidget->setParent(NULL);
}
