/*
 * Flame.h
 *
 *  Created on: 4 oct. 2010
 *      Author: lartick
 */

#ifndef FLAME_H_
#define FLAME_H_

#include <QObject>
#include <QTimer>
#include <QPoint>
#include <QDataStream>

class Flame : public QObject {
	Q_OBJECT
public:
	static int index;
	int playerId;
	void addFlame(int x, int y);
	QList<QPoint*> getFlamePositions() const;
	Flame();
	Flame(int playerId, int duration);
	virtual ~Flame();
private:
	QTimer *blinkTimer;
	QList<QPoint*> flames;
	void flameEnd();
signals:
	void flameTimeOut(Flame & f);
};
QDataStream &operator>>(QDataStream & in, Flame &f);
QDataStream &operator<<(QDataStream &out, const Flame &f);


#endif /* FLAME_H_ */
