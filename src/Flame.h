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


/**
  This method is necessary in order to use QPoint in a QSet
  */
uint qHash(const QPoint& key);


/**
  This class is used to store all the information related to the explosion of a bomb.

  We store here the list of all cases in which a flame appears, as well as the list of
  blocks broken by the fire. In the future there should also be the list of destroyed bonus.
 */
class Flame : public QObject {
	Q_OBJECT

public:
	Flame();

	/**
	  Constructor

	  @param playerId Id of the player to whom belongs the initial bomb
	  @param duration Lifespan of the flames in number of heartbeats
	  */
	Flame(int playerId, int duration);
	virtual ~Flame();

	void addFlame(int x, int y);
	QSet<QPoint> getFlamePositions() const		{ return flames; }

	void addBrokenBlock(int x, int y);
	QSet<QPoint>::const_iterator getFirstBrokenBlock() const	{ return brokenBlocks.constBegin(); }
	QSet<QPoint>::const_iterator getLastBrokenBlock() const		{ return brokenBlocks.constEnd(); }

	int getFlameId() const						{ return flameId; }

	int getPlayerId() const						{ return playerId; }

	void decreaseLifeSpan()						{ duration--; }
	bool isFinished()							{ return duration < 0; }

private:
	static int index;							///< unique ID for each explosion

	int flameId;
	int playerId;								///< owner of the bomb

	int duration;								///< duration in number of heartbeats

	QSet<QPoint> flames;
	QSet<QPoint> brokenBlocks;

private:
	// allows easier serialization, avoids exposing all internal variables for writing
	friend QDataStream &operator>>(QDataStream & in, Flame &f);
	friend QDataStream &operator<<(QDataStream &out, const Flame &f);

};

QDataStream &operator>>(QDataStream & in, Flame &f);
QDataStream &operator<<(QDataStream &out, const Flame &f);


#endif /* FLAME_H */
