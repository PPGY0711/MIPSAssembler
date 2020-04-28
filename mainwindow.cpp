#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "highlight.h"
#include <QPushButton>
#include <QTextEdit>
#include <QTextBrowser>
#include <QFormLayout>
#include <QTextCursor>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MIPS Simulator @3160104633@zju.edu.cn");
//    ui->ExecuteBtn->setDisabled(true);
//    ui->DebugBtn->setDisabled(true);
    ui->ControlPanel->setReadOnly(true);
    ui->MemoryWindow->setReadOnly(true);
    ui->ProgramWindow->setReadOnly(true);
    //启动模拟器
    reBootComputer();
    //初始化界面
    ui->PCWindow->setWindowTitle("MainWindow");
    ui->PCWindow->setFont(QFont(tr("宋体"),12));
    ui->ControlPanel->setWindowTitle("ControlPanel");
    ui->ControlPanel->setFont(QFont(tr("Consolas"),10));
    ui->MemoryWindow->setFont(QFont(tr("Consolas"),9));
    ui->ProgramWindow->setFont(QFont(tr("Consolas"),10));

    Highlight *inputAsmHighLight = new Highlight(ui->MIPSAsmInput->document());
    ui->MIPSAsmInput->clear();
    ui->MachineCodeInput->clear();
    //表格内容初始化
    ui->RegisterFile->setRowCount(8);
    ui->RegisterFile->setColumnCount(4);
    ui->RegisterFile->setWindowTitle("Register File");
    QStringList header;
    header << "0" << "1" << "2" << "3";
    ui->RegisterFile->setHorizontalHeaderLabels(header);
    for(int i = 0; i < 4; i++){
        QTableWidgetItem *columnHeaderItem = ui->RegisterFile->horizontalHeaderItem(i); //获得水平方向表头的Item对象
        columnHeaderItem->setFont(QFont(tr("Consolas"),10)); //设置字体
        columnHeaderItem->setBackgroundColor(QColor(0,60,10)); //设置单元格背景颜色
        columnHeaderItem->setTextColor(QColor(200,111,30)); //设置文字颜色
    }
    ui->RegisterFile->verticalHeader()->setVisible(false);//隐藏左边行号
    ui->RegisterFile->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setTableContent();
    ui->RegisterFile->show();
    ui->MIPSAsmInput->setFont(QFont(tr("Consolas"),10));
    ui->MachineCodeInput->setFont(QFont(tr("Consolas"),10));
    this->isDebugBtnDown = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reBootComputer(){
    printf("---------------------------reBoot Running!-----------------------------\n");
    this->mpc = buildComputer();
    showMemory();
    showProgram();
    showDisplay();
    showCtrlPanel();
    setTableContent();
    ui->ExecuteBtn->setDisabled(false);
    ui->ExecuteMachineCode->setDisabled(false);
    ui->DebugBtn->setDisabled(false);
    ui->pushButton->setDisabled(false);
    /*ui->MIPSAsmInput->clear();
    ui->MachineCodeInput->clear();*/
    printf("---------------------------reBoot End!-----------------------------\n");
}

void MainWindow::showMemory(){
    printf("---------------------------ShowMemory Running!-----------------------------\n");
    this->MemoryStr = "";
    for(int i = 0; i < MEMORYSIZE; i++){
        if(i%16==0){
            char tmpaddr[9];
            sprintf(tmpaddr,"%08x",i);
            string tmpaddrs(tmpaddr);
            this->MemoryStr += " [" +tmpaddrs.substr(4,4)+"]: ";
        }
        char tmpCell[9];
        sprintf(tmpCell,"%08x",this->mpc.MemoryMap[i]);
        string tmpCellStr(tmpCell);
        this->MemoryStr = this->MemoryStr + tmpCellStr.substr(4,4) + " ";

        if((i+1)%8==0 && (i+1)%16 != 0){
            this->MemoryStr += "- ";
        }
        if((i+1)%8==0 && (i+1)%16 == 0){
            this->MemoryStr += "\n";
        }
    }
    ui->MemoryWindow->setPlainText(QString(this->MemoryStr.c_str()));
//    printf("str print try: %s\n","你好");
//    printf("MemoryStr:\n%s\n",this->MemoryStr);
    printf("---------------------------ShowMemory End!-----------------------------\n");
}

void MainWindow::showProgram(){
    //把目前内存（不包括显存）中的数据以汇编指令的形式显示
    printf("---------------------------ShowProgram Running!-----------------------------\n");
    this->CodeStr = "";
    for(int i = 0; i < MEMORYSIZE-DISPLAYVOLMUE; i+=2){
        unsigned int mcode = 0;
        mcode |= mpc.MemoryMap[i];
        mcode <<= 16;
        mcode |= mpc.MemoryMap[i+1];
        string bStr = udex2binstr(mcode);
        this->CodeStr += bStr + "\n";
    }
//    printf("Codestr:%s\n",this->CodeStr);
    this->CodeStr = disassembler(this->CodeStr);
//    printf("---------------------------------Code Start-------------------------------------\n");
//    printf("%s\n",this->CodeStr);
//    printf("---------------------------------Code End-------------------------------------\n");
    vector<string> codelines = splitC(this->CodeStr,'\n');
//    printf("code size:%d\n",codelines.size());
    this->codeTbl.clear();
    for(int i = 0; i< codelines.size();i++){
        string codePC = codelines[i].substr(0,codelines[i].find('['));
        codePC = trim(codePC);
        unsigned int cpc = strtoul(codePC.c_str(),NULL,10);
        string ccode = codelines[i].substr(codelines[i].find(']')+1);
        this->codeTbl.insert(pair<unsigned int, string>(cpc,ccode));
    }
    ui->ProgramWindow->setPlainText(QString(this->CodeStr.c_str()));
    printf("---------------------------ShowProgram Running!-----------------------------\n");
}

void MainWindow::showDisplay(){
    //读取显存中的数据形成字符串
    printf("---------------------------ShowDisplay Running!-----------------------------\n");
    this->mainWindowStr = "";
    for(int i = MEMORYSIZE-DISPLAYVOLMUE; i < MEMORYSIZE; i++){
        char cs[3];
        cs[0] = mpc.MemoryMap[i] >> 8;
        cs[1] = mpc.MemoryMap[i] & 0xFF;
        cs[2] = '\0';
        if(cs[1] != '\0'){
            if((cs[0] & 0x80) == 0){
                string scell(cs+1);
                this->mainWindowStr += scell;
//                printf("char %d: %d\n",i-0x3000,cs[1]);
            }
            else{
                string scell(cs);
                this->mainWindowStr += scell;
//                printf("char %d: %02x%02x\n",i-0x3000,cs[0],cs[1]);
            }
        }
        else
            break;
    }
//    printf("displayStr: %s\n",this->mainWindowStr);

    ui->PCWindow->setPlainText(QString::fromLocal8Bit(this->mainWindowStr.c_str()));
    printf("---------------------------ShowDisplay End!-----------------------------\n");
}

void MainWindow::showCtrlPanel(){
    printf("---------------------------ShowCtrlPanel Running!-----------------------------\n");
    unsigned int codeinMem = 0;
    codeinMem |= this->mpc.MemoryMap[this->mpc.lastPC];
    codeinMem <<= 16;
    codeinMem |= this->mpc.MemoryMap[this->mpc.lastPC+1];
    unsigned int opc = codeinMem >> 26;
//    unsigned int func = codeinMem & 0x0000003F;
    unsigned int rsnum = (codeinMem & 0x03E00000) >> 21;
    unsigned int rtnum = (codeinMem & 0x001F0000) >> 16;
    unsigned int rdnum = (codeinMem & 0x0000F800) >> 11;
    unsigned int sanum = (codeinMem & 0x000007C0) >> 6;
    unsigned int adr = (this->mpc.PC & 0xF8000000) + ((codeinMem & 0x03FFFFFF) << 1);
    int imme = (codeinMem&0xFFFF) << 16;
    imme >>= 16;
    this->ControlPanelStr = "-------Control Panel-------\n";
    this->ControlPanelStr += "[PC   ]: " + StrToHex(udex2binstr(this->mpc.PC)) + "\n";
    char buf[40];
    memset(buf,0,sizeof(char)*40);
    int dis = 0;
    string rs,rt,rd;
    rs = this->mpc.rgs->dRegWordTbl[rsnum];
    rt = this->mpc.rgs->dRegWordTbl[rtnum];
    rd = this->mpc.rgs->dRegWordTbl[rdnum];

    //instr
    this->ControlPanelStr += "[instr]: " + this->codeTbl[this->mpc.lastPC] +"\n";

    //op
    sprintf(buf,"[op   ]: %d\n",opc);
    string cell2(buf);
    this->ControlPanelStr += cell2;
    memset(buf,0,sizeof(char)*40);
    //rs
    this->ControlPanelStr += "[";
    dis = 5 - rs.length();
    while(dis){
        rs = " " +rs;
        dis--;
    }
    this->ControlPanelStr += rs + "]" + ": " + udex2str(this->mpc.rgs->regContent[rsnum]) + "\n";

    //rt
    this->ControlPanelStr += "[";
    dis = 5 - rt.length();
    while(dis){
        rt = " " +rt;
        dis--;
    }
    this->ControlPanelStr += rt + "]" + ": " + udex2str(this->mpc.rgs->regContent[rtnum]) + "\n";

    //rd
    this->ControlPanelStr += "[";
    dis = 5 - rd.length();
    while(dis){
        rd = " " +rd;
        dis--;
    }
    this->ControlPanelStr += rd + "]" + ": " + udex2str(this->mpc.rgs->regContent[rdnum]) + "\n";

    //shmt
    sprintf(buf,"[shmt ]: %d\n",sanum);
    string cell6(buf);
    this->ControlPanelStr += cell6;
    memset(buf,0,sizeof(char)*40);

    //data
    sprintf(buf,"[data ]: [    %04x]%d\n",imme,imme);
    string cell7(buf);
    this->ControlPanelStr += cell7;
    memset(buf,0,sizeof(char)*40);

    //addr
    sprintf(buf,"[addr ]: [ %07x]%u\n",adr,adr);
    string cell8(buf);
    this->ControlPanelStr += cell8;
    memset(buf,0,sizeof(char)*40);

    //memory
    sprintf(buf,"[Memry]: [%08x]%u\n",codeinMem,codeinMem);
    string cell9(buf);
    this->ControlPanelStr += cell9;
    memset(buf,0,sizeof(char)*40);
    ui->ControlPanel->setPlainText(QString(this->ControlPanelStr.c_str()));
    printf("---------------------------ShowCtrlPanel Running!-----------------------------\n");
}

void MainWindow::setTableContent(){
    printf("---------------------------SetUpTableContent Running!-----------------------------\n");
    int row,col;
    row = col = 0;
    for(col = 0; col < 4; col++){
        for(row = 0; row < 8; row++){
            char tmpc[10];
            sprintf(tmpc,"%08x",this->mpc.rgs->regContent[col*8+row]);
            string showStr(tmpc);
            showStr = this->mpc.rgs->dRegWordTbl[col*8+row] + ": "+ showStr;
            QTableWidgetItem* item =  new QTableWidgetItem(QString(showStr.c_str()));
            item->setFont(QFont(tr("Consolas"),9));
            ui->RegisterFile->setItem(row,col,item);
        }
    }
    printf("---------------------------SetUpTableContent End!-----------------------------\n");
}

void MainWindow::ExecuteProgram(string program, int choice){
    //choice == 0,输入为MIPS汇编代码输入
    //choice == 1,输入为二进制机器码输入
    ExecuteCode(this->mpc,program,choice);
    showMemory();
    showProgram();
    showCtrlPanel();
    showDisplay();
    setTableContent();
}

void MainWindow::DebugProgram(string program, int choice){
    //choice == 0,输入为MIPS汇编代码输入
    //choice == 1,输入为二进制机器码输入   
    DebugCode(this->mpc,program,choice);
    showCtrlPanel();
    showMemory();
    showProgram();
//    showDisplay();
    setTableContent();
}

void MainWindow::on_ResetBtn_clicked()
{
    reBootComputer();
}

void MainWindow::on_ExecuteBtn_clicked()
{
    string program;
    if(ui->MIPSAsmInput->document()->isModified() || !ui->MIPSAsmInput->document()->isEmpty()){
        program = ui->MIPSAsmInput->toPlainText().toStdString();
        reBootComputer();
        ExecuteProgram(program,0);
    }
}

void MainWindow::on_ExecuteMachineCode_clicked()
{
    string program;
    if(ui->MachineCodeInput->document()->isModified() || !ui->MachineCodeInput->document()->isEmpty()){
        program = ui->MachineCodeInput->toPlainText().toStdString();
        ui->MachineCodeInput->setReadOnly(true);
        reBootComputer();
        ExecuteProgram(program,1);
    }
    ui->MachineCodeInput->setReadOnly(false);
}

void MainWindow::on_DebugBtn_clicked()
{
    string program;
    if(ui->MIPSAsmInput->document()->isModified() || !ui->MIPSAsmInput->document()->isEmpty()){
        program = ui->MIPSAsmInput->toPlainText().toStdString();
        ui->MIPSAsmInput->setReadOnly(true);
        if(this->isDebugBtnDown == 0){
            resetRegisterFile(&this->mpc);
            setTableContent();
            resetPC(&this->mpc);
            DebugProgram(program,0);
            showCtrlPanel();
            ui->ExecuteBtn->setDisabled(true);
            ui->ExecuteMachineCode->setDisabled(true);
            this->isDebugBtnDown = 1;
        }
        else{
            DebugProgram(program,0);
            if(this->mpc.isDebugBegin == 0){
                ui->MIPSAsmInput->setReadOnly(false);
                ui->ExecuteBtn->setDisabled(false);
                ui->ExecuteMachineCode->setDisabled(false);
                this->isDebugBtnDown = 0;
            }
        }
        showDisplay();
    }
}

void MainWindow::on_pushButton_clicked()
{
    string program;
    if(ui->MIPSAsmInput->document()->isModified() || !ui->MIPSAsmInput->document()->isEmpty()){
        program = ui->MIPSAsmInput->toPlainText().toStdString();
        ui->MIPSAsmInput->setReadOnly(true);
        if(this->isDebugBtnDown == 0){
            resetRegisterFile(&this->mpc);
            setTableContent();
            resetPC(&this->mpc);
            DebugProgram(program,1);
            showCtrlPanel();
            ui->ExecuteBtn->setDisabled(true);
            ui->ExecuteMachineCode->setDisabled(true);
            this->isDebugBtnDown = 1;
        }
        else{
            DebugProgram(program,1);
            if(this->mpc.isDebugBegin == 0){
                ui->MIPSAsmInput->setReadOnly(false);
                ui->ExecuteBtn->setDisabled(false);
                ui->ExecuteMachineCode->setDisabled(false);
                this->isDebugBtnDown = 0;
            }
        }
        showDisplay();
    }
}
