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

#define NET_VERSION 4 //update this number if network compatibility is broken

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
};

#endif

