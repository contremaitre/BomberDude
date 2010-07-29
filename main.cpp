#include <QApplication>
#include "Board.h" 

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Board pl(20,20,30);
    return app.exec();
}

