#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "stimulate.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void reBootComputer();
    void showMemory();
    void showProgram();
    void showHelloWorld();
    void showCtrlPanel();
    void ExecuteProgram(string program, int choice);
    void DebugProgram(string program, int choice);
private:
    Ui::MainWindow *ui;
    Mipsc mpc;
    string mainWindowStr,MemoryStr,ControlPanelStr,CodeStr;
    map<unsigned int, string> codeTbl;
};

#endif // MAINWINDOW_H
