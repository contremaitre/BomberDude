#ifndef QTB_POINT_H
#define QTB_POINT_H


#include <QDataStream>
#include <QHash>


template<typename T>
class Point
{
public:
    Point()         : m_x(0), m_y(0)    {}
    Point(T x, T y) : m_x(x), m_y(y)    {}

    T x() const         { return m_x; }
    T y() const         { return m_y; }

private:
    T m_x;
    T m_y;

//	friend QDataStream& operator>>(QDataStream& in, Point<T>& p);
//	friend QDataStream& operator<<(QDataStream& out, const Point<T>& p);
};


//template<typename T>
//QDataStream& operator>>(QDataStream& in, Point<T>& p) {
//    in >> p.m_x >> p.m_y;
//    return in;
//}

//template<typename T>
//QDataStream& operator<<(QDataStream& out, const Point<T>& p) {
//    out << p.m_x << p.m_y;
//    return out;
//}


template<typename T>
bool operator==(const Point<T>& p1, const Point<T>& p2) {
    return p1.x() == p2.x() && p1.y() == p2.y();
}

template<typename T>
bool operator<(const Point<T>& p1, const Point<T>& p2) {
    if(p1.x() == p2.x())
        return p1.y() < p2.y();
    return p1.x() < p2.x();
}

template<typename T>
uint qHash(const Point<T>& p) {
    return qHash(qHash(p.x()) ^ p.y());
}


#endif // QTB_POINT_H
