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


#ifndef CONSTANT_H_
#define CONSTANT_H_


//constants or defaults values
#define VERSION 0.1

//Graphics
#define BLOCK_SIZE 30
#define MAP_SIZE 20

#define MAP_PATH "maps/"

//Game
#define MAX_PLAYERS 4 //initial max player value when creating a game
#define MAX_WINS 5 //initial max wins to end a tournament
#define MAX_NB_PLAYER 4 //for random maps
#define DEFAULT_BOMB_DURATION 40 //ticks
#define DEFAULT_BOMB_DUD_MALUS 3 //dud bomb duration = DEFAULT_BOMB_DURATION * DEFAULT_BOMB_DUD_MALUS
#define DEFAULT_BOMB_RANGE 2
#define DEFAULT_BOMB_CAPACITY 1
#define DEFAULT_DISEASE_DURATION 150
#define DEFAULT_GAME_DURATION 180 //seconds
#define SHRINKING_SPEED 10//heartbeat
#define SPAWNING_BLOCK_INTERVAL 10 //seconds
//how fast the bomb is blinking (ms)

enum globalDirection {
    dirNone = -1,
    dirLeft = 0,
    dirUp = 1,
    dirRight = 2,
    dirDown = 3,
};

//Network
#define SERVER_PORT 10000
#define SERVER_ADDRESS "localhost" 
#define UDP_STATS_INTERVAL 150

//Gameplay
#define HEARTBEAT 50 //(ms)
#define MOVE_STEP (int)(BLOCK_SIZE/6)
#define WALKWAY_SPEED 2
const int ROLLING_BOMB_SPEED = MOVE_STEP * 2;
#define OIL_BOMB_RANDOM_DIR 3 //chances for a jelly bomb to change direction (1 = 100%)
const int MAX_PLAYER_NAME_LENGTH = 16;

//BONUS. A negative number here indicates how many times we'll do a 1-in-10 chance
// of putting the powerup down.
#define BONUS_NB_BOMBS 10
#define BONUS_NB_FLAMES 10
#define BONUS_NB_DISEASES 3
#define BONUS_NB_KICKERS 4
#define BONUS_NB_SKATES 8
#define BONUS_NB_PUNCH 2
#define BONUS_NB_GRAB 2
#define BONUS_NB_MULTIBOMB 1
#define BONUS_NB_GOLDFLAME -2
#define BONUS_NB_TRIGGER 4
#define BONUS_NB_OIL 4
#define BONUS_NB_SUPERDISEASES -4
#define BONUS_NB_RANDOM -2

const char SIGNAL_SERVER_LISTENING[] = "Server is listening";

#endif // CONSTANT_H_
