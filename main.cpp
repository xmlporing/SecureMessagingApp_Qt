//#include "mainwindow.h"
#include <QApplication>
#include "manager.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); //solve the button issue
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    Manager m;

    return a.exec();
}
