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


#ifndef QBOMB_H
#define QBOMB_H

#include "QAnimateditem.h"
#include <QDebug>

class QBomb : public QAnimatedItem
{
private:
    void initPixList();

public:
    QBomb();
    QBomb(int,int,int);
    static QList<QPixmap*> normalPix;
    static QList<QPixmap*> rcPix;
    static QList<QPixmap*> dudPix;
    void setRC();
    void setNormalBomb();
    void setDudBomb();
};

#endif // QBOMB_H
