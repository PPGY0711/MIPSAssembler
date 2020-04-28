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
    void showDisplay();
    void showCtrlPanel();
    void setTableContent();
    void ExecuteProgram(string program, int choice);
    void DebugProgram(string program, int choice);

private slots:
    void on_ResetBtn_clicked();

    void on_ExecuteBtn_clicked();

    void on_ExecuteMachineCode_clicked();

    void on_DebugBtn_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    Mipsc mpc;
    string mainWindowStr,MemoryStr,ControlPanelStr,CodeStr;
    map<unsigned int, string> codeTbl;
    int isDebugBtnDown;
};

#endif // MAINWINDOW_H
