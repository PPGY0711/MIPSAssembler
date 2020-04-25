#include "mainwindow.h"
#include "stimulate.h"
#include "mnemonic.h"
#include "register.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
