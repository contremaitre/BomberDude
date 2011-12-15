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

#ifndef QTB_SETTINGS_H
#define QTB_SETTINGS_H


#include <QObject>
#include <QSettings>


struct key_set_t
{
    Qt::Key left;
    Qt::Key up;
    Qt::Key down;
    Qt::Key right;
    Qt::Key drop;
    Qt::Key opt;
};

class Settings : public QObject
{
    Q_OBJECT
private:
    QSettings *qsettings;
    int serverPort;
    QString serverAddress;
    bool b_isServer;
    bool sound;
    QString playerName;
    bool debugMode;
    key_set_t player1Keys;
    key_set_t player2Keys;

public:
    Settings();
    ~Settings();

    void save();

    bool isServer() const;
    QString getServerAddress() const;
    int getServerPort() const;
    bool isSound() const;
    QString getPlayerName() const;
    bool isDebugMode() const;
    key_set_t getPlayer1Keys() const;
    key_set_t getPlayer2Keys() const;

    void setServer(bool);
    void setServerPort(int);
    void setServerAddress(const QString&);
    void setSound(bool);
    void setPlayerName(const QString&);
    void setDebugMode(bool);
    void setPlayer1Keys(const key_set_t&);
    void setPlayer2Keys(const key_set_t&);

    void loadDefaultKeys();
};


#endif // QTB_SETTINGS_H
