#include "InterGameFrame.h"
#include "PlayerListWidget.h"

InterGameFrame::InterGameFrame(PlayerListWidget *playerListWidget, bool admin, QFrame *parent)
                        : QFrame(parent), playerListWidget(playerListWidget)
{
    ui.setupUi(this);
    playerListWidget->setParent(ui.playerListWidgetHolder);
    ui.nextRoundButton->setEnabled(admin);
}

InterGameFrame::~InterGameFrame()
{
    playerListWidget->setParent(NULL);
}
