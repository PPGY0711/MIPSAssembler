#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QCloseEvent>

class QLineEdit;
class QDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void newFile();
    bool maybeSave();
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    bool loadFile(const QString &fileName);
    std::string getcurFileName();
    bool saveProcessedFile();
    void buildAssembler(int outType,int choice);
protected:
    void closeEvent(QCloseEvent *event);
private slots://定义按钮动作函数
    void on_actionNewFile_N_triggered();

    void on_actionSave_S_triggered();

    void on_actionSave_As_A_triggered();

    void on_actionOpenFile_O_triggered();

    void on_actionClose_C_triggered();

    void on_actionUndo_U_triggered();

    void on_actionCut_T_triggered();

    void on_actionCopy_C_triggered();

    void on_actionPaste_P_triggered();

    void on_actionExit_X_triggered();

    void showFindLastText();

    void showFindNextText();

    void replaceSingleText();

    void replaceAllText();

    void on_actionFind_F_triggered();

    void on_actionDelete_L_triggered();

    void on_actionReplace_R_triggered();

    void on_actionSelectAll_A_triggered();


    void on_actionBin_triggered();

    void on_actionHex_triggered();

    void on_actionSaveMachineCode_triggered();

private:
    Ui::MainWindow *ui;
    bool isUntitled;
    QString curFile;
    QString outFile;
    QLineEdit *findLineEdit;
    QDialog *findDlg;
    QLineEdit *replaceLineEdit;
    QLineEdit *replaceWord;
    QDialog *replaceDlg;
};

#endif // MAINWINDOW_H
