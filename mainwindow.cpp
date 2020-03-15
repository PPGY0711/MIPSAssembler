#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "assembler.h"
#include "highlight.h"
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QTextEdit>
#include <QLineEdit>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextCursor>
#include <iostream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->rawInput->setVisible(false);
    ui->processedOut->setVisible(false);

    ui->rawInput->setFont(QFont(tr("Consolas"),10));
    ui->processedOut->setFont(QFont(tr("Consolas"),10));
    Highlight *inputhighlight = new Highlight(ui->rawInput->document());
    Highlight *outputhighlight = new Highlight(ui->processedOut->document());

    setWindowTitle("MIPS assembler #3160104633@zju.edu.cn");
    isUntitled = true;
    curFile = tr("Untitled.asm");
    ui->actionBin->setDisabled(true);
    ui->actionHex->setDisabled(true);

    //for search
    findDlg = new QDialog(this);
    findDlg->setWindowTitle(tr("Search"));
    findLineEdit = new QLineEdit(findDlg);
    QPushButton *lastbtn = new QPushButton(tr("Search for Last"),findDlg);
    QPushButton *nextbtn = new QPushButton(tr("Search for Next"),findDlg);
    QVBoxLayout *flayout = new QVBoxLayout(findDlg);
    flayout->addWidget(findLineEdit);
    flayout->addWidget(lastbtn);
    flayout->addWidget(nextbtn);
    connect(lastbtn,&QPushButton::clicked,this,&MainWindow::showFindLastText);
    connect(nextbtn,&QPushButton::clicked,this,&MainWindow::showFindNextText);

    //for replace
    replaceDlg = new QDialog(this);
    replaceDlg->setWindowTitle(tr("Replace"));
    replaceLineEdit = new QLineEdit(replaceDlg);
    replaceWord = new QLineEdit(replaceDlg);
    QPushButton *singlebtn = new QPushButton(tr("Replace"),replaceDlg);
    QPushButton *allbtn = new QPushButton(tr("Replace All"),replaceDlg);
    QVBoxLayout *rlayout = new QVBoxLayout(replaceDlg);
    rlayout->addWidget(replaceLineEdit);
    rlayout->addWidget(replaceWord);
    rlayout->addWidget(singlebtn);
    rlayout->addWidget(allbtn);
    connect(singlebtn,&QPushButton::clicked,this,&MainWindow::replaceSingleText);
    connect(allbtn,&QPushButton::clicked,this,&MainWindow::replaceAllText);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile()
{
    if(maybeSave())
    {
        isUntitled = true;
        curFile = tr("Untitled.asm");
        setWindowTitle(curFile);
        ui->rawInput->clear();
        ui->processedOut->clear();
        ui->rawInput->setVisible(true);
        ui->processedOut->setVisible(true);
        ui->processedOut->setReadOnly(true);
        ui->actionBin->setDisabled(false);
        ui->actionHex->setDisabled(false);
    }
}

bool MainWindow::maybeSave()
{
    if(ui->rawInput->document()->isModified())
    {
        QMessageBox warning;
        warning.setWindowTitle(tr("Warning"));
        warning.setIcon(QMessageBox::Warning);
        warning.setText(curFile + tr(" hasn't been saved yet. Save it or not?"));
        QPushButton *yesBtn = warning.addButton(tr("Yes(&Y)"),QMessageBox::YesRole);
        warning.addButton(tr("No(&N)"),QMessageBox::NoRole);
        QPushButton *cancelBtn = warning.addButton(tr("Cancel"),QMessageBox::RejectRole);
        warning.exec();
        if(warning.clickedButton() == yesBtn)
            return save();
        else if(warning.clickedButton() == cancelBtn)
            return false;
    }
    return true;
}

bool MainWindow::save()
{
    if(isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save as"),curFile);
    if(fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("MIPS Assembler & Disassembler"),
                             tr("cannot write to file %1:\n %2")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out<<ui->rawInput->toPlainText();
    QApplication::restoreOverrideCursor();
    isUntitled = false;
    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle("MIPS assembler & disassembler #3160104633@zju.edu.cn");
    return true;
}

void MainWindow::on_actionNewFile_N_triggered()
{
    newFile();
}

void MainWindow::on_actionSave_S_triggered()
{
    save();
}

void MainWindow::on_actionSave_As_A_triggered()
{
    saveAs();
}

bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("MIPS Assembler"),
                             tr("cannot read file %1:\n %2")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }
    //func;
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->rawInput->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    curFile = QFileInfo(fileName).canonicalFilePath();
    setWindowTitle(curFile);
    ui->actionBin->setDisabled(false);
    ui->actionHex->setDisabled(false);
    return true;
}

void MainWindow::on_actionOpenFile_O_triggered()
{
    if(maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this,tr("Open File"),"",tr("ASM(*.asm *.txt);;Text Files(*.bin *.coe)"));
        if(!fileName.isEmpty())
        {
            loadFile(fileName);
            ui->rawInput->setVisible(true);
            ui->processedOut->setVisible(true);
            ui->processedOut->setReadOnly(true);
        }
    }
}

void MainWindow::on_actionClose_C_triggered()
{
    if(maybeSave())
    {
        ui->rawInput->setVisible(false);
        ui->processedOut->setVisible(false);
        ui->actionBin->setDisabled(true);
        ui->actionHex->setDisabled(true);
    }
}

void MainWindow::on_actionUndo_U_triggered()
{
    ui->rawInput->undo();
}

void MainWindow::on_actionCut_T_triggered()
{
    ui->rawInput->cut();
}

void MainWindow::on_actionCopy_C_triggered()
{
    ui->rawInput->copy();
}

void MainWindow::on_actionPaste_P_triggered()
{
    ui->rawInput->paste();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::on_actionExit_X_triggered()
{
    on_actionClose_C_triggered();
    qApp->quit();
}

void MainWindow::showFindLastText()
{
    QString str = findLineEdit->text();
    if(!ui->rawInput->find(str,QTextDocument::FindBackward))
    {
        QMessageBox::warning(this,tr("Search"),tr("Cannot find %1 in this file.").arg(str));
    }
}

void MainWindow::showFindNextText()
{
    QString str = findLineEdit->text();
    if(!ui->rawInput->find(str))
    {
        QMessageBox::warning(this,tr("Search"),tr("Cannot find %1 in this file.").arg(str));
    }
}

void MainWindow::on_actionFind_F_triggered()
{
    findDlg->show();
}


void MainWindow::on_actionDelete_L_triggered()
{
    ui->rawInput->textCursor().removeSelectedText();
}

void MainWindow::replaceSingleText()
{
    QTextCursor tmpCursor = ui->rawInput->textCursor();
    QString str = replaceLineEdit->text();
    QString rstr = replaceWord->text();

    if(!ui->rawInput->find(str,QTextDocument::FindBackward))
    {
        QMessageBox::warning(this,tr("Warning"),tr("Cannot find target string."));
    }
    else
    {
        ui->rawInput->textCursor().removeSelectedText();
        ui->rawInput->textCursor().insertText(rstr);
        //QMessageBox::warning(this,tr("Replace Single"),tr("Replace completed."));
    }
}

void MainWindow::replaceAllText()
{
    QString str = replaceLineEdit->text();
    QString rstr = replaceWord->text();
    QString passage = ui->rawInput->toPlainText();
    passage.replace(QString(str),QString(rstr));
    ui->rawInput->clear();
    ui->rawInput->setPlainText(passage);
    QMessageBox::warning(this,tr("Replace All"),tr("Replace completed."));
}

void MainWindow::on_actionReplace_R_triggered()
{
    replaceDlg->show();
}


void MainWindow::on_actionSelectAll_A_triggered()
{
    ui->rawInput->selectAll();
}

std::string MainWindow::getcurFileName()
{
    return curFile.toStdString();
}

void MainWindow::buildAssembler(int outType,int choice)
{
    string fileName,content;
    string result;
    bool status;
    if(!ui->rawInput->document()->isModified()){
        fileName = getcurFileName();
        status = assembler(fileName,"",0,result,outType);
        }
    else
    {//bool assembler(string filename, string content, int choice, string &result, int FileType)
        content = ui->rawInput->toPlainText().toStdString();
        cout<<content<<endl;
        status = assembler("",content,1,result,outType);
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->processedOut->setReadOnly(false);
    ui->processedOut->clear();
    ui->processedOut->setPlainText(QString(result.c_str()));
    QApplication::restoreOverrideCursor();
    ui->processedOut->setReadOnly(true);
}

bool MainWindow::saveProcessedFile()
{
    QString defaultName;
    QString fileName;
    defaultName = "Untitled.bin";
    fileName = QFileDialog::getSaveFileName(this,tr("Save as .bin"),defaultName,tr("BIN Files (*.bin)"));

    QFile file(fileName);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("MIPS Assembler"),
                             tr("cannot write to file %1:\n %2")
                             .arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out<<ui->processedOut->toPlainText();
    QApplication::restoreOverrideCursor();
    return true;
}


void MainWindow::on_actionBin_triggered()
{
    buildAssembler(0,0);
}

void MainWindow::on_actionHex_triggered()
{
    buildAssembler(1,0);
}

void MainWindow::on_actionSaveMachineCode_triggered()
{
    saveProcessedFile();
}
