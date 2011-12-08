/*
    Copyright (C) 2010,2011 Sébastien Escudier

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


#include "Settings.h"
#include "constant.h"

#include <QDebug>

#define DEFAULT_KEY_1_LEFT Qt::Key_Left
#define DEFAULT_KEY_1_RIGHT Qt::Key_Right
#define DEFAULT_KEY_1_UP Qt::Key_Up
#define DEFAULT_KEY_1_DOWN Qt::Key_Down
#define DEFAULT_KEY_1_DROP Qt::Key_Control
#define DEFAULT_KEY_1_OPT Qt::Key_Shift

#define DEFAULT_KEY_2_LEFT Qt::Key_S
#define DEFAULT_KEY_2_RIGHT Qt::Key_F
#define DEFAULT_KEY_2_UP Qt::Key_E
#define DEFAULT_KEY_2_DOWN Qt::Key_D
#define DEFAULT_KEY_2_DROP Qt::Key_Space
#define DEFAULT_KEY_2_OPT Qt::Key_Alt

Settings::Settings()
{
    qsettings = new QSettings("QtBomberman", "QtBomberman");
    qsettings->beginGroup("Network");
    b_isServer = qsettings->value("isServer",false).toBool();
    serverAddress = qsettings->value("serverAddress",SERVER_ADDRESS).toString();
    serverPort = qsettings->value("serverPort",SERVER_PORT).toInt();
    sound = qsettings->value("sound", true).toBool();
    playerName = qsettings->value("playerName", "player").toString();
    if(playerName.isEmpty())
        playerName = "player"; //the user may have set an empty name
    debugMode = qsettings->value("debugMode", false).toBool();
    qsettings->endGroup();

    /* Keys */
    qsettings->beginGroup("Control Keys");
    qsettings->beginGroup("Player 1");
    player1Keys.left = static_cast<Qt::Key>(qsettings->value("key_left", DEFAULT_KEY_1_LEFT).toInt());
    player1Keys.right = static_cast<Qt::Key>(qsettings->value("key_right", DEFAULT_KEY_1_RIGHT).toInt());
    player1Keys.up = static_cast<Qt::Key>(qsettings->value("key_up", DEFAULT_KEY_1_UP).toInt());
    player1Keys.down = static_cast<Qt::Key>(qsettings->value("key_down", DEFAULT_KEY_1_DOWN).toInt());
    player1Keys.drop = static_cast<Qt::Key>(qsettings->value("key_drop", DEFAULT_KEY_1_DROP).toInt());
    player1Keys.opt = static_cast<Qt::Key>(qsettings->value("key_opt", DEFAULT_KEY_1_OPT).toInt());
    qsettings->endGroup();
    qsettings->beginGroup("Player 2");
    player2Keys.left = static_cast<Qt::Key>(qsettings->value("key_left", DEFAULT_KEY_2_LEFT).toInt());
    player2Keys.right = static_cast<Qt::Key>(qsettings->value("key_right", DEFAULT_KEY_2_RIGHT).toInt());
    player2Keys.up = static_cast<Qt::Key>(qsettings->value("key_up", DEFAULT_KEY_2_UP).toInt());
    player2Keys.down = static_cast<Qt::Key>(qsettings->value("key_down", DEFAULT_KEY_2_DOWN).toInt());
    player2Keys.drop = static_cast<Qt::Key>(qsettings->value("key_drop", DEFAULT_KEY_2_DROP).toInt());
    player2Keys.opt = static_cast<Qt::Key>(qsettings->value("key_opt", DEFAULT_KEY_2_OPT).toInt());
    qsettings->endGroup();
    qsettings->endGroup();
}

void Settings::save()
{
    qsettings->beginGroup("Network");
    qsettings->setValue("isServer",b_isServer);
    qsettings->setValue("sound",sound);
    qsettings->setValue("serverAddress",serverAddress);
    qsettings->setValue("serverPort",serverPort);
    qsettings->setValue("playerName", playerName);
    qsettings->setValue("debugMode", debugMode);
    qsettings->endGroup();

    qsettings->beginGroup("Control Keys");
    qsettings->beginGroup("Player 1");
    qsettings->setValue("key_left", player1Keys.left);
    qsettings->setValue("key_right", player1Keys.right);
    qsettings->setValue("key_up", player1Keys.up);
    qsettings->setValue("key_down", player1Keys.down);
    qsettings->setValue("key_drop", player1Keys.drop);
    qsettings->setValue("key_opt", player1Keys.opt);
    qsettings->endGroup();
    qsettings->beginGroup("Player 2");
    qsettings->setValue("key_left", player2Keys.left);
    qsettings->setValue("key_right", player2Keys.right);
    qsettings->setValue("key_up", player2Keys.up);
    qsettings->setValue("key_down", player2Keys.down);
    qsettings->setValue("key_drop", player2Keys.drop);
    qsettings->setValue("key_opt", player2Keys.opt);
    qsettings->endGroup();
    qsettings->endGroup();
}

bool Settings::isServer() const
{
    return b_isServer;
}

QString Settings::getServerAddress() const
{
    return serverAddress;
}

int Settings::getServerPort() const
{
    return serverPort;
}

QString Settings::getPlayerName() const {
    return playerName;
}

bool Settings::isSound() const
{
    return sound;
}

bool Settings::isDebugMode() const {
    return debugMode;
}

key_set_t Settings::getPlayer1Keys() const {
    return player1Keys;
}
key_set_t Settings::getPlayer2Keys() const {
    return player2Keys;
}


void Settings::setServer(bool val)
{
    b_isServer = val;
}

void Settings::setServerPort(int port)
{
    serverPort = port;
}

void Settings::setServerAddress(const QString& addr)
{
    serverAddress = addr;
}

void Settings::setSound(bool val)
{
    sound = val;
}

void Settings::setPlayerName(const QString& name) {
    playerName = name;
}

void Settings::setDebugMode(bool val) {
    debugMode = val;
}

Settings::~Settings()
{
    save();
    delete qsettings;
}

