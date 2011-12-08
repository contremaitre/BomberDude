#include <QKeySequence>
#include <QKeyEvent>
#include "KeySettingsWidget.h"
#include "Settings.h"

#include <QDebug>
KeySettingsWidget::KeySettingsWidget(Settings *settings,QWidget *parent)
                        : QWidget(parent), settings(settings)
{
    ui.setupUi(parent);
    loadKeys();

    connect(ui.upButton1,SIGNAL(clicked()),this,SLOT(slotPl1Up()));
    keyListening = false;
    this->setFocusPolicy(Qt::ClickFocus);

}

void KeySettingsWidget::loadKeys()
{
    key_set_t keys = settings->getPlayer1Keys();
    ui.key_left_1->setText(keyToString(keys.left));
    ui.key_right_1->setText(keyToString(keys.right));
    ui.key_down_1->setText(keyToString(keys.down));
    ui.key_up_1->setText(keyToString(keys.up));
    ui.key_drop_1->setText(keyToString(keys.drop));
    ui.key_opt_1->setText(keyToString(keys.opt));

    keys = settings->getPlayer2Keys();
    ui.key_left_2->setText(keyToString(keys.left));
    ui.key_right_2->setText(keyToString(keys.right));
    ui.key_down_2->setText(keyToString(keys.down));
    ui.key_up_2->setText(keyToString(keys.up));
    ui.key_drop_2->setText(keyToString(keys.drop));
    ui.key_opt_2->setText(keyToString(keys.opt));

}

void KeySettingsWidget::slotPl1Up()
{
    qDebug() << "change pl1 up";
    listenKey();
}

void KeySettingsWidget::listenKey()
{
    keyListening = true;
    this->setFocus(Qt::MouseFocusReason);
}

void KeySettingsWidget::keyPressEvent(QKeyEvent *event)
{
    if(keyListening)
    {
        //keyListening = false;
        int key = event->key();
        qDebug() << "key press event" << keyToString(key);
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

QString KeySettingsWidget::keyToString(int key)
{

    /* These Keys are not set in QKeySequence */
    if(key == Qt::Key_Shift)
        return "Shift";
    else if(key == Qt::Key_Control)
        return "Ctrl";
    else if(key == Qt::Key_Alt)
        return "Alt";

    return QKeySequence(key).toString();
}

KeySettingsWidget::~KeySettingsWidget()
{
}
