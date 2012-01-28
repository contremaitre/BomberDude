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

#include "Bonus.h"

QDataStream& operator>>(QDataStream& in, Bonus& b) {
    in >> b.type >> b.x >> b.y;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Bonus& b) {
    Q_ASSERT(b.type != Bonus::BONUS_NONE);
    out << b.type << b.x << b.y;
    return out;
}
