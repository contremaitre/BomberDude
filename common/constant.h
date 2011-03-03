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


#ifndef CONSTANT_H_
#define CONSTANT_H_


//constants that may be later removed or changed in a configuration menu

//Graphics
#define BLOCK_SIZE 30
#define MAP_SIZE 20

#define MAP_PATH "maps/"

//Game
#define MAX_NB_PLAYER 4 //for random maps
#define DEFAULT_BOMB_DURATION 40 //ticks
#define DEFAULT_BOMB_RANGE 2
#define DEFAULT_BOMB_CAPACITY 1
#define DEFAULT_DISEASE_DURATION 150
#define DEFAULT_GAME_DURATION 10 //seconds
#define SHRINKING_SPEED 2//heartbeat, must be the same value client and server side
//how fast the bomb is blinking (ms)

//Network
#define SERVER_PORT 10000
#define SERVER_ADDRESS "localhost" 
#define UDP_STATS_INTERVAL 150

//Gameplay
#define HEARTBEAT 50 //(ms)
#define MOVE_STEP (int)(BLOCK_SIZE/6)
#define WALKWAY_SPEED 2

const int MAX_PLAYER_NAME_LENGTH = 16;


#endif // CONSTANT_H_
