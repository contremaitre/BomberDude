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

#include <QSettings>
#include "Settings.h"
#include "constant.h"

Settings::Settings()
{
    qsettings = new QSettings("QtBomberman", "QtBomberman");
    qsettings->beginGroup("Network");
    isServer = qsettings->value("isServer",false).toBool();
    serverAddress = qsettings->value("serverAddress",SERVER_ADDRESS).toString();
    serverPort = qsettings->value("serverPort",SERVER_PORT).toInt();
    sound = qsettings->value("sound", true).toBool();
    showIpStats = qsettings->value("showIpStats", true).toBool();
}

void Settings::save()
{
    qsettings->setValue("isServer",isServer);
    qsettings->setValue("sound",sound);
    qsettings->setValue("serverAddress",serverAddress);
    qsettings->setValue("serverPort",serverPort);
    qsettings->setValue("showIpStats",showIpStats);
}

bool Settings::getServer() const
{
    return isServer;
}

bool Settings::getShowIpStats() const
{
    return showIpStats;
}

QString Settings::getServerAddress() const
{
    return serverAddress;
}

int Settings::getServerPort() const
{
    return serverPort;
}

bool Settings::isSound() const
{
    return sound;
}

void Settings::setServer(bool val)
{
    isServer = val;
}

void Settings::setShowIpStats(bool val)
{
    showIpStats = val;
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

Settings::~Settings()
{
    save();
    delete qsettings;
}

