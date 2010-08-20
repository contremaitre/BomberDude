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

enum NetMsgType
{
   msg_move = 0,
   msg_moved,
};

struct NetHeader
{
   int length;
   NetMsgType type;
};

struct NetMsgMove : NetHeader
{
    //Client->Server send the direction of the move
    int direction;
};

struct NetMsgMoved : NetHeader
{
    //Server->Clients send the new postition on the map
    int player;
    int position;
};

#endif

