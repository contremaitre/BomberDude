#include <QApplication>
#include "GameField.h" 

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GameField pl(10,10,15);
    return app.exec();
}

