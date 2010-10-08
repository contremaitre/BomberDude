/*
    Copyright (C) 2010 lartick

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

#ifndef FLAME_H
#define FLAME_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QPoint>
#include <QDataStream>

class Flame : public QObject {
	Q_OBJECT
	static int index;
public:
	int playerId;
	void addFlame(int x, int y);
	QList<QPoint> getFlamePositions() const;
	Flame();
	Flame(int playerId, int duration);
	virtual ~Flame();
	void setFlameId(int id);
	int getFlameId() const;
	void startFlameTimer();
private:
	int flameId;
	QTimer blinkTimer;
	QList<QPoint> flames;
	int duration;
signals:
	void flameEnd(Flame&);
	void test();
public slots:
	void flameTimeout();

};
QDataStream &operator>>(QDataStream & in, Flame &f);
QDataStream &operator<<(QDataStream &out, const Flame &f);


#endif /* FLAME_H */
