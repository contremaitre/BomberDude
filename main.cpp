#include <QApplication>
#include "GameField.h" 

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GameField pl(20,20,30);
    return app.exec();
}

