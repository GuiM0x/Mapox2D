#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q_INIT_RESOURCE(mapox2d);
    Q_INIT_RESOURCE(stylesheets);

    MainWindow w;
    w.show();

    return a.exec();
}
