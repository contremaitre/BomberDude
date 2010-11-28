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
