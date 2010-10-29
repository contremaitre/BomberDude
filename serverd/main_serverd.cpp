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
#include "constant.h"
#include "Serverd.cpp"
#include "constant.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv, false);

    Serverd *serverd;
    const char *mapFile = NULL;
    if(argc > 2 && !strcmp(argv[1],"--mapfile"))
        mapFile = argv[2];

    if(mapFile)
        serverd = new Serverd(SERVER_PORT,mapFile);
    else
        serverd = new Serverd(SERVER_PORT,MAP_SIZE,BLOCK_SIZE);

    serverd->launch();
    app.exec();

    delete serverd;
    return 0;
}

