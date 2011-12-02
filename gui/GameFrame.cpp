#include "GameFrame.h"
#include "IpStats.h"

GameFrame::GameFrame(IpStats *ipStats, QFrame *parent) : QFrame(parent), ipStats(ipStats)
{
    ui.setupUi(this);
    ipStats->setParent(ui.IpStatHolder);
}

GameFrame::~GameFrame()
{
    ipStats->setParent(NULL);
}
