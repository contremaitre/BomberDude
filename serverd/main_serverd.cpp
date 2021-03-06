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
#include <iostream>
#include <sstream>

#include "Serverd.h"
#include "constant.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv, false);

    const char *adminPasswd = NULL;
	int portNumber = SERVER_PORT;
    bool debugMode = false;
    bool startedFromGui = false;

	int index_arg = 1;

	// parse the command line parameters
	// each option section must increment index_arg by itself and finish by "continue"
	while(index_arg < argc) {
		if(strcmp(argv[index_arg], "--port") == 0) {
			if(++index_arg >= argc) {
				std::cerr << "Parameter --port: missing argument" << std::endl;
				return 42;
			}
			std::istringstream iss(argv[index_arg++]);
			iss >> portNumber;
			if(portNumber < 1 || portNumber > 65535) {
				std::cerr << "Parameter --port: invalid port number " << portNumber << std::endl;
				return 42;
			}
			continue;
		}

        if(strcmp(argv[index_arg], "--admin-password") == 0) {
            if(++index_arg >= argc) {
                std::cerr << "Parameter --admin-password: missing argument" << std::endl;
                return 42;
            }
            adminPasswd = argv[index_arg++];
            continue;
        }

        if(strcmp(argv[index_arg], "--debug-mode") == 0) {
            debugMode = true;
            ++index_arg;
            continue;
        }
        
        if(strcmp(argv[index_arg], "--started-from-gui") == 0) {
            startedFromGui = true;
            ++index_arg;
            continue;
        }

		std::cout << "Unknown parameter: " << argv[index_arg] << std::endl;
		++index_arg;
	}

	std::cout << "Using port number: " << portNumber << std::endl;

    Serverd *serverd = new Serverd( portNumber,
                                    adminPasswd,
                                    MAP_SIZE,
                                    BLOCK_SIZE,
                                    debugMode,
                                    startedFromGui);

    QObject::connect(serverd,SIGNAL(sigQuit()), &app, SLOT(quit()));
    serverd->launch();
    app.exec();

    delete serverd;
    return 0;
}

