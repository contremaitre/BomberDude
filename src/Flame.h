/*
 * Flame.h
 *
 *  Created on: 4 oct. 2010
 *      Author: lartick
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
	QList<QPoint*> getFlamePositions() const;
	Flame();
	Flame(int playerId, int duration);
	virtual ~Flame();
	void setFlameId(int id);
	int getFlameId() const;
	void startFlameTimer();
private:
	int flameId;
	QTimer *blinkTimer;
	QList<QPoint*> flames;
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
