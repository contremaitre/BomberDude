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

/**
 *
 * Define the message structures between NetClient and NetServer
 *
 */

#ifndef NETMESSAGE_H
#define NETMESSAGE_H

#define NET_VERSION 9 //update this number if network compatibility is broken

enum NetMsgType
{
    msg_move = 0,
    msg_moved,
    msg_udp_welcome,
    msg_map,
    msg_bomb,
    msg_ping,
    msg_udp_stat,
	msg_update_map,
	msg_net_version,
	msg_is_admin,
	msg_max_players,        // TCP client <-> server, set the max number of players
    msg_start_game,         // TCP client <-> server start game, and game started
    msg_player_data,        // TCP client -> server, player's profile
    msg_update_player_data, // TCP server -> client, player's profile
    msg_admin_passwd,       // TCP client -> server, admin password
    msg_shutdown_server,    // TCP client -> server, Note : must be logged on the server with an admin password
    msg_select_map,         // TCP client -> server, select next map in the list
    msg_map_preview,        // TCP server -> client, map preview
    msg_map_random,         // TCP server -> client, random map
    msg_map_winner,         // TCP server -> client, map's winner + statistics
    msg_client_disconnected,// TCP server -> client, player left
    msg_opt_key,            // UDP client -> server, key option 1
    msg_server_stopped,     // TCP server -> client, the server has stopped (shut down or admin GUI crashed)
    msg_kick_player,        // TCP client -> server, kick a player
    msg_max_wins,           // TCP client <-> server, set the max number of wins in a tournament
};

#endif
