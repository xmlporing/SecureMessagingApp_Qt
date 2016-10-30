//#include "mainwindow.h"
#include <QApplication>
#include <QLoggingCategory>
#include "manager.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); //solve the button issue
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false"); // solve the qt.network.ssl: Error receiving trust for a CA certificate warning
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    Manager m;

    return a.exec();
}
