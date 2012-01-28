/*
    Copyright (C) 2011 Sébastien Escudier

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

#include <QKeySequence>
#include <QKeyEvent>
#include "KeySettingsWidget.h"

#include <QDebug>
KeySettingsWidget::KeySettingsWidget(Settings *settings,QWidget *parent)
                        : QWidget(parent), settings(settings)
{
    ui.setupUi(parent);
    loadKeys();
    connect(ui.upButton1,SIGNAL(clicked()),this,SLOT(slotPl1Up()));
    connect(ui.upButton2,SIGNAL(clicked()),this,SLOT(slotPl2Up()));
    connect(ui.rightButton1,SIGNAL(clicked()),this,SLOT(slotPl1Right()));
    connect(ui.rightButton2,SIGNAL(clicked()),this,SLOT(slotPl2Right()));
    connect(ui.leftButton1,SIGNAL(clicked()),this,SLOT(slotPl1Left()));
    connect(ui.leftButton2,SIGNAL(clicked()),this,SLOT(slotPl2Left()));
    connect(ui.downButton1,SIGNAL(clicked()),this,SLOT(slotPl1Down()));
    connect(ui.downButton2,SIGNAL(clicked()),this,SLOT(slotPl2Down()));
    connect(ui.dropButton1,SIGNAL(clicked()),this,SLOT(slotPl1Drop()));
    connect(ui.dropButton2,SIGNAL(clicked()),this,SLOT(slotPl2Drop()));
    connect(ui.optButton1,SIGNAL(clicked()),this,SLOT(slotPl1Opt()));
    connect(ui.optButton2,SIGNAL(clicked()),this,SLOT(slotPl2Opt()));
    connect(ui.defaultButton, SIGNAL(clicked()), this, SLOT(slotDefault()));

    stopListening();
    memset(&listenedKey,0, sizeof(listenedKey));
    listenedPlayer = 0;
    this->setFocusPolicy(Qt::ClickFocus);

}

void KeySettingsWidget::loadKeys()
{
    player1_keys = settings->getPlayer1Keys();
    ui.key_left_1->setText(keyToString(player1_keys.left));
    ui.key_right_1->setText(keyToString(player1_keys.right));
    ui.key_down_1->setText(keyToString(player1_keys.down));
    ui.key_up_1->setText(keyToString(player1_keys.up));
    ui.key_drop_1->setText(keyToString(player1_keys.drop));
    ui.key_opt_1->setText(keyToString(player1_keys.opt));

    player2_keys = settings->getPlayer2Keys();
    ui.key_left_2->setText(keyToString(player2_keys.left));
    ui.key_right_2->setText(keyToString(player2_keys.right));
    ui.key_down_2->setText(keyToString(player2_keys.down));
    ui.key_up_2->setText(keyToString(player2_keys.up));
    ui.key_drop_2->setText(keyToString(player2_keys.drop));
    ui.key_opt_2->setText(keyToString(player2_keys.opt));

}

void KeySettingsWidget::slotPl1Up()
{
    listenKey("player 1 up");
    listenedPlayer = 1;
    listenedKey.up = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl1Right()
{
    listenKey("player 1 right");
    listenedPlayer = 1;
    listenedKey.right = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl1Left()
{
    listenKey("player 1 left");
    listenedPlayer = 1;
    listenedKey.left = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl1Down()
{
    listenKey("player 1 down");
    listenedPlayer = 1;
    listenedKey.down = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl1Drop()
{
    listenKey("player 1 drop");
    listenedPlayer = 1;
    listenedKey.drop = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl1Opt()
{
    listenKey("player 1 opt");
    listenedPlayer = 1;
    listenedKey.opt = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl2Up()
{
    listenKey("player 2 up");
    listenedPlayer = 2;
    listenedKey.up = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl2Right()
{
    listenKey("player 2 right");
    listenedPlayer = 2;
    listenedKey.right = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl2Left()
{
    listenKey("player 2 left");
    listenedPlayer = 2;
    listenedKey.left = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl2Down()
{
    listenKey("player 2 down");
    listenedPlayer = 2;
    listenedKey.down = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl2Drop()
{
    listenKey("player 2 drop");
    listenedPlayer = 2;
    listenedKey.drop = Qt::Key_Escape;
}

void KeySettingsWidget::slotPl2Opt()
{
    listenKey("player 2 opt");
    listenedPlayer = 2;
    listenedKey.opt = Qt::Key_Escape;
}

void KeySettingsWidget::listenKey(const QString& text)
{
    keyListening = true;
    ui.listenKeyLabel->setText(QString("Press ") + text + QString(" key"));
    this->setFocus(Qt::MouseFocusReason);
    memset(&listenedKey,0, sizeof(listenedKey));
}

void KeySettingsWidget::stopListening()
{
    keyListening = false;
    ui.listenKeyLabel->setText("");
}

void KeySettingsWidget::keyPressEvent(QKeyEvent *event)
{
    if(keyListening)
    {
        //qDebug() << "key press event" << keyToString(event->key());
        changeKey(event->key());
        stopListening();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void KeySettingsWidget::changeKey(int k)
{
    Qt::Key key = static_cast<Qt::Key>(k);
    if(listenedPlayer == 1)
    {
        if(listenedKey.up != 0)
        {
            player1_keys.up = key;
            ui.key_up_1->setText(keyToString(key));
        }
        else if(listenedKey.right != 0)
        {
            player1_keys.right = key;
            ui.key_right_1->setText(keyToString(key));
        }
        else if(listenedKey.left != 0)
        {
            player1_keys.left = key;
            ui.key_left_1->setText(keyToString(key));
        }
        else if(listenedKey.down != 0)
        {
            player1_keys.down = key;
            ui.key_down_1->setText(keyToString(key));
        }
        else if(listenedKey.drop != 0)
        {
            player1_keys.drop = key;
            ui.key_drop_1->setText(keyToString(key));
        }
        else if(listenedKey.opt != 0)
        {
            player1_keys.opt = key;
            ui.key_opt_1->setText(keyToString(key));
        }
        settings->setPlayer1Keys(player1_keys);
    }
    else
    {
        if(listenedKey.up != 0)
        {
            player2_keys.up = key;
            ui.key_up_2->setText(keyToString(key));
        }
        else if(listenedKey.right != 0)
        {
            player2_keys.right = key;
            ui.key_right_2->setText(keyToString(key));
        }
        else if(listenedKey.left != 0)
        {
            player2_keys.left = key;
            ui.key_left_2->setText(keyToString(key));
        }
        else if(listenedKey.down != 0)
        {
            player2_keys.down = key;
            ui.key_down_2->setText(keyToString(key));
        }
        else if(listenedKey.drop != 0)
        {
            player2_keys.drop = key;
            ui.key_drop_2->setText(keyToString(key));
        }
        else if(listenedKey.opt != 0)
        {
            player2_keys.opt = key;
            ui.key_opt_2->setText(keyToString(key));
        }
        settings->setPlayer2Keys(player2_keys);
    }
}


void KeySettingsWidget::focusOutEvent(QFocusEvent *event)
{
    stopListening();
}

void KeySettingsWidget::slotDefault()
{
    settings->loadDefaultKeys();
    loadKeys();
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
