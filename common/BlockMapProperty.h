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

#ifndef BLOCKMAPPROPERTY_H
#define BLOCKMAPPROPERTY_H

class BlockMapProperty
{
public:
    //must be kept in sync in PixmapsItem
    enum BlockType {wall,brick,bomb,flame,empty,broken};
    enum Option {none,o_bomb};

    BlockMapProperty();
    ~BlockMapProperty();

    void setType(BlockType);
    BlockType getType() const;
private:
    BlockType type;
    Option option;

};

#endif

