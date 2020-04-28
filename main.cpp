#include "mainwindow.h"
#include "iostream"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    cout<<"show window"<<endl;
    w.show();

    return a.exec();
}
