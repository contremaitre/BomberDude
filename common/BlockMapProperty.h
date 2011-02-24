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

#ifndef QTB_BLOCKMAPPROPERTY_H
#define QTB_BLOCKMAPPROPERTY_H


/**
 * This class represent a block on the field
 * A block can be a wall, a brick or empty
 * A block can also have an option like an arrow, a teleport etc...
 */
class BlockMapProperty
{
public:
    //must be kept in sync in PixmapsItem
    enum BlockType {wall,brick,empty,broken,flame};
    enum BlockOption {none,arrow,teleport,mov_walk};

    BlockMapProperty();
    ~BlockMapProperty();

    void setType(BlockType);
    BlockType getType() const;
private:
    BlockType type;
    BlockOption option;
};


#endif // QTB_BLOCKMAPPROPERTY_H
