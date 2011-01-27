#ifndef QTB_BONUS_H
#define QTB_BONUS_H


#include <QDataStream>


class Bonus {
public:
    typedef quint8 Bonus_t;
    static const Bonus_t BONUS_NONE = 0;         // for bogus object (to be initialised by deserialisation)
    static const Bonus_t BONUS_FLAME = 1;
    static const Bonus_t BONUS_BOMB = 2;
    static const Bonus_t BONUS_OIL = 3;
    static const Bonus_t BONUS_DISEASE = 4;

    Bonus() :
        type(BONUS_NONE),
        x(-1),
        y(-1)
    {}

    Bonus(Bonus_t pType, qint16 pX, qint16 pY) :
        type(pType),
        x(pX),
        y(pY)
    {}

    Bonus_t getType() const                 { Q_ASSERT(type != BONUS_NONE); return type; }

    qint16 getX() const                     { Q_ASSERT(type != BONUS_NONE); return x; }
    void setX(qint16 pX)                    { Q_ASSERT(type != BONUS_NONE); x = pX; }
    
    qint16 getY() const                     { Q_ASSERT(type != BONUS_NONE); return y; }
    void setY(qint16 pY)                    { Q_ASSERT(type != BONUS_NONE); y = pY; }

private:
    Bonus_t type;
    qint16 x;
    qint16 y;

    friend QDataStream& operator>>(QDataStream& in, Bonus& b);
    friend QDataStream& operator<<(QDataStream& out, const Bonus& b);
};


QDataStream& operator>>(QDataStream& in, Bonus& b);
QDataStream& operator<<(QDataStream& out, const Bonus& b);


#endif // QTB_BONUS_H
