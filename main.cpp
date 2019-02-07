//#include "mainwindow.h"
#include "customwindow.h"
#include <QApplication>

///////////////////////////////////////////////
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*Q_INIT_RESOURCE(mapox2d);
    Q_INIT_RESOURCE(stylesheets);*/

    CustomWindow cw;
    cw.show();

    return a.exec();
}
