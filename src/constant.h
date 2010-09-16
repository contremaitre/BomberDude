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

<<<<<<< HEAD:src/constant.h
//constants that may be later removed or changed in a configuration menu

//Graphics
#define BLOCK_SIZE 30
#define MAP_SIZE 20

//Game
#define MAX_NB_PLAYER 2
#define DEFAULT_BOMB_DURATION 4000 //ms
#define DEFAULT_BOMB_RANGE 1
//how fast the bomb is blinking (ms)
#define BOMB_BLINK_INTERVAL 500

//Network
#define SERVER_PORT 10000
#define SERVER_ADDRESS "localhost" 
#define LOCAL_ADDRESS "127.0.0.1"

//Gameplay
#define MOVE_TICK_INTERVAL 50 //(ms)
#define MOVE_STEP (int)(BLOCK_SIZE/7)
=======
/**
 *
 * Define the message structures between NetClient and NetServer
 *
 */

#ifndef NETMESSAGE_H
#define NETMESSAGE_H

enum NetMsgType
{
    msg_move = 0,
    msg_moved,
    msg_udp_welcome,
    msg_map,
    msg_ping,
};

#endif
>>>>>>> UDP Ping every 2s:NetMessage.h

