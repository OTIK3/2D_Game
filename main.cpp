#include "angbandgame.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AngBangGame w;
    return a.exec();
}
