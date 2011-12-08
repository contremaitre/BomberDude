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

#include <QApplication>
#include "startUi.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    StartUi startui(&app);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(&startui);
    scrollArea->resize(850+10,650+10); //todo, find a better way to set the initial size of this scroll area
    scrollArea->show();
    //startui.show();
    app.exec();

    return 0;
}

