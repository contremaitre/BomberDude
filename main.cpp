#include <QApplication>
#include "Board.h" 

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Board pl(5,10);
    return app.exec();
}

