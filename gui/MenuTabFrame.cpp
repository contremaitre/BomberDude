#include <QtGui>
#include <QSound>

#include "MenuTabFrame.h"
#include "constant.h"
#include "Settings.h"
#include "IpStats.h"
#include "PlayerListWidget.h"
#include "KeySettingsWidget.h"
#include "NetClient.h"

MenuTabFrame::MenuTabFrame(Settings *settings, IpStats *ipStats, PlayerListWidget *playerListWidget, QSound *music, QFrame *parent)
    : QFrame(parent), settings(settings), ipStats(ipStats), playerListWidget(playerListWidget), music(music)
{
    TabUi.setupUi(this);
    keySettingsWidget = new KeySettingsWidget(settings,TabUi.Keys);
    //KeysUi.setupUi(TabUi.Keys);
    MainUi.setupUi(TabUi.Main);
    MainUi.playerName->setMaxLength(MAX_PLAYER_NAME_LENGTH);
    loadNetWidget();
    loadSound();
    loadPlayerData();
    connect(MainUi.isServer, SIGNAL(stateChanged(int)), this, SLOT(isServerChanged(int)));
    connect(MainUi.sound, SIGNAL(stateChanged(int)), this, SLOT(soundChanged(int)));
    connect(MainUi.startGameButton,SIGNAL(clicked()),this,SLOT(slotStartButton()));
    connect(MainUi.disconnectButton,SIGNAL(clicked()),this,SLOT(slotDisconnectButton()));
    connect(MainUi.serverButton,SIGNAL(clicked()),this,SLOT(slotStartServer()));
    connect(MainUi.kickButton,SIGNAL(clicked()),this,SLOT(slotKickButton()));

    //connect(MainUi.stopGame, SIGNAL(clicked()), this, SLOT(closeGame())); TODO

    ipStats->setParent(MainUi.IpStatHolder);
    playerListWidget->setParent(MainUi.playerListWidgetHolder);
    QSize size2(MainUi.serverStatus->width(), MainUi.serverStatus->height());
    QPixmap load = QPixmap("pictures/loading.png");
    loading = load.scaled(size2);
}

void MenuTabFrame::loadSound()
{
    if(!QSound::isAvailable())
    {
        qDebug() << "No Sound available";
        MainUi.sound->setEnabled(false);
        MainUi.sound->setCheckState(Qt::Unchecked);
        return;
    }
    Qt::CheckState checked = settings->isSound() ? Qt::Checked : Qt::Unchecked;
    MainUi.sound->setCheckState(checked);
    toggleMusic(settings->isSound());
}

void MenuTabFrame::toggleMusic(bool on)
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

void MenuTabFrame::loadNetWidget()
{
    Qt::CheckState checked = settings->isServer() ? Qt::Checked : Qt::Unchecked;

    MainUi.isServer->setCheckState(checked);
    MainUi.serverIp->setPlainText(settings->getServerAddress());
    QString port;
    port.setNum(settings->getServerPort());
    MainUi.serverPort->setPlainText(QString(port));
    updateNetWidgetState(checked == 0);
    MainUi.serverStatus->setPixmap(NULL);
    MainUi.maxPlayersBox->setValue(0);
    MainUi.adminWidget->setEnabled(false);
    MainUi.randomMapCheck->setCheckState(Qt::Unchecked);
    MainUi.mapRightButton->setEnabled(true);
    MainUi.mapLeftButton->setEnabled(true);
    MainUi.checkDebugMode->setChecked(settings->isDebugMode());
}

void MenuTabFrame::loadPlayerData() {
    MainUi.playerName->setText(settings->getPlayerName());
}

bool MenuTabFrame::setSettings()
{
    bool ok, checked;
    int port = MainUi.serverPort->toPlainText().toInt(&ok);
    if(!ok)
        return false;
    checked = MainUi.isServer->checkState() == 0 ? false : true;
    settings->setServer(checked);
    settings->setServerPort(port);
    settings->setServerAddress(MainUi.serverIp->toPlainText());
    checked = MainUi.sound->checkState() == 0 ? false : true;
    settings->setSound(checked);
    settings->setPlayerName(MainUi.playerName->text());
    settings->setDebugMode(MainUi.checkDebugMode->isChecked());
    return true;
}

void MenuTabFrame::updateNetWidgetState(bool en)
{
    MainUi.serverIp->setEnabled(en);
    MainUi.ipLabel->setEnabled(en);
    MainUi.passwordLabel->setEnabled(en);
    MainUi.password->setEnabled(en);
    if(en)
    {
        MainUi.serverButton->setText("Connect");
    }
    else
    {
        MainUi.serverButton->setText("Launch");
    }
}

void MenuTabFrame::slotConnectedToServer()
{
    MainUi.serverStatus->setPixmap(loading);
}

void MenuTabFrame::slotStartServer()
{
    MainUi.network_pref->setEnabled(false);
    MainUi.disconnectButton->setEnabled(true);
    MainUi.addLocalPlayerButton->setEnabled(true);
}

void MenuTabFrame::slotIsServerAdmin()
{
    MainUi.maxPlayersBox->setMinimum(1);
    MainUi.adminWidget->setEnabled(true);
}

void MenuTabFrame::slotMaxPlayersValueChanged(int maxPlayers)
{
    MainUi.maxPlayersBox->setValue(maxPlayers);
}

void MenuTabFrame::slotPlayerLeft(qint8 playerId)
{
    delete MainUi.playersList->takeItem(playerId,0);
    delete MainUi.playersList->takeItem(playerId,1);
}

void MenuTabFrame::slotKickButton()
{
    QList<QTableWidgetItem *> list = MainUi.playersList->selectedItems ();
    if( !list.empty())
    {
        QTableWidgetItem *item = list.first();
        int num = item->text().toInt();
        emit sigKickPlayer(num);
    }
}

void MenuTabFrame::slotMapRandom(bool rand)
{
    if(rand)
    {
        MainUi.randomMapCheck->setCheckState(Qt::Checked);
        MainUi.mapRightButton->setEnabled(false);
        MainUi.mapLeftButton->setEnabled(false);
    }
    else
    {
        MainUi.randomMapCheck->setCheckState(Qt::Unchecked);
        MainUi.mapRightButton->setEnabled(true);
        MainUi.mapLeftButton->setEnabled(true);
    }

}

void MenuTabFrame::slotDisconnectButton()
{
    emit sigDisconnect();
}

void MenuTabFrame::slotStartButton()
{
    emit sigStart(MainUi.mapOptionCBox->currentIndex());
}

void MenuTabFrame::slotUpdatePlayerData(qint8 playerId, QString playerName) {
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(playerId));
    MainUi.playersList->setItem(playerId, 0, newItem);
    newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    newItem = new QTableWidgetItem(playerName);
    newItem->setFlags(Qt::ItemIsEnabled);
    MainUi.playersList->setItem(playerId, 1, newItem);
}

void MenuTabFrame::slotMapPreviewReceived(MapClient *map)
{
    const QList<mapStyle> *styles = map->getStyles();
    MainUi.mapOptionCBox->clear();
    MainUi.mapOptionCBox->addItem ( "no style" );
    foreach(mapStyle s, *styles)
        MainUi.mapOptionCBox->addItem ( s.name );
}

void MenuTabFrame::isServerChanged(int state)
{
    updateNetWidgetState(state == 0);
}

void MenuTabFrame::soundChanged(int state)
{
    toggleMusic(state != 0);
}

const QString MenuTabFrame::getPlayerName() const
{
    return MainUi.playerName->text();
}

QString MenuTabFrame::getAdminPassword() const
{
    return MainUi.password->toPlainText();
}

QGraphicsView *MenuTabFrame::getGraphicPreview() const
{
    return MainUi.previewGraphicsView;
}

MenuTabFrame::~MenuTabFrame()
{
    //todo destroy player list
    ipStats->setParent(NULL);
    playerListWidget->setParent(NULL);
}
