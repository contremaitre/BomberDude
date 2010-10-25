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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
private:
    QSettings *qsettings;
    int serverPort;
    QString serverAddress;
    bool isServer;
    bool sound;
    bool showIpStats;

public:
    Settings();
    ~Settings();

    bool getServer() const;
    QString getServerAddress() const;
    int getServerPort() const;
    bool isSound() const;
    bool getShowIpStats() const;
    void save();
    void setServer(bool);
    void setServerPort(int);
    void setServerAddress(const QString&);
    void setSound(bool);
    void setShowIpStats(bool);

};

#endif

