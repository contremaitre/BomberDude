#include "GameFrame.h"
#include "IpStats.h"
#include "PlayerListWidget.h"

GameFrame::GameFrame(IpStats *ipStats, PlayerListWidget *playerListWidget, QFrame *parent)
                        : QFrame(parent), ipStats(ipStats), playerListWidget(playerListWidget)
{
    ui.setupUi(this);
    ipStats->setParent(ui.IpStatHolder);
    playerListWidget->setParent(ui.playerListWidgetHolder);
}

GameFrame::~GameFrame()
{
    ipStats->setParent(NULL);
    playerListWidget->setParent(NULL);
}
