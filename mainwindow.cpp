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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MIPS Simulator @3160104633@zju.edu.cn");
    ui->ExecuteBtn->setDisabled(true);
    ui->DebugBtn->setDisabled(true);
    ui->ControlPanel->setReadOnly(true);
    ui->MemoryWindow->setReadOnly(true);
    ui->ProgramWindow->setReadOnly(true);
    ui->RegisterFile->setEditTriggers(QAbstractItemView.NoEditTriggers);
    ui->RegisterFile = QTableWidget(8,4);
    ui->RegisterFile->setHorizontalHeaderLabels({'0','1','2','3'});
    //启动模拟器
    reBootComputer();
    //初始化界面
    ui->PCWindow->setFont(QFont(tr("宋体"),12));
    ui->PCWindow->setText(this->mainWindowStr);
    ui->ControlPanel->setFont(QFont(tr("Consolas"),12));
    ui->ControlPanel->setText(this->ControlPanelStr);
    ui->MemoryWindow->setFont(QFont(tr("Consolas"),12));
    ui->MemoryWindow->setText(this->MemoryStr);
    ui->ProgramWindow->setFont(QFont(tr("Consolas"),12));
    ui->ProgramWindow->setText(this->CodeStr);
    Highlight *inputAsmHighLight = new Highlight(ui->MIPSAsmInput->document());
    ui->MIPSAsmInput->clear();
    ui->MachineCodeInput->clear();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reBootComputer(){
    this->mpc = buildComputer();
    showMemory();
    showProgram();
    showHelloWorld();
    showCtrlPanel();
}

void MainWindow::showMemory(){
    this->MemoryStr = "";
    for(int i = 0; i < MEMORYSIZE; i++){
        char tmpCell[9];
        sprintf(tmpCell,"%08x",this->mpc.MemoryMap[i]);
        string tmpCellStr(tmpCell);
        MemoryStr = MemoryStr + tmpCellStr.substr(4,4) + " ";
        if((i+1)%8==0 && (i+1)%16 != 0){
            MemoryStr += "- ";
        }
        if((i+1)%8==0 && (i+1)%16 == 0){
            MemoryStr += "\n";
        }
    }
}

void MainWindow::showProgram(){
    //把目前内存（不包括显存）中的数据以汇编指令的形式显示
    this->CodeStr = "";
    for(int i = 0; i < MEMORYSIZE-DISPLAYVOLMUE; i+=2){
        unsigned int mcode = 0;
        mcode |= mpc.MemoryMap[i];
        mcode <<= 16;
        mcode |= mpc.MemoryMap[i+1];
        bitset<32> mcodebit(mcode);
        this->CodeStr += mcodebit.to_string<char, std::string::traits_type, std::string::allocator_type>()+"\n";
    }
    this->CodeStr = disassembler(this->CodeStr);
    vector<string> codelines = splitC(this->CodeStr,'\n');
    this->codeTbl.clear();
    for(int i = 0; i< codelines.size();i++){
        unsigned int cpc = strtoul(trim(codelines[i].substr(0,codelines[i].find('['))),NULL,10);
        string ccode = codelines[i].substr(codelines[i].find('[')+1,codelines[i].find(']')-codelines[i].find('[')-1);
        this->codeTbl.insert(pair<unsigned int, string>(cpc,ccode));
    }
}

void MainWindow::showHelloWorld(){
    //读取显存中的数据形成字符串
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
            }
            else{
                string scell(cs);
                this->mainWindowStr += scell;
            }
        }
        else
            break;
    }
}

void MainWindow::showCtrlPanel(){
    unsigned int codeinMem = 0;
    codeinMem |= this->mpc.MemoryMap[this->mpc.PC];
    codeinMem <<= 16;
    codeinMem |= this->mpc.MemoryMap[this->mpc.PC+1];
    unsigned int opc = mcode >> 26;
    unsigned int func = mcode & 0x0000003F;
    unsigned int rsnum = (mcode & 0x03E00000) >> 21;
    unsigned int rtnum = (mcode & 0x001F0000) >> 16;
    unsigned int rdnum = (mcode & 0x0000F800) >> 11;
    unsigned int sanum = (mcode & 0x000007C0) >> 6;
    unsigned int adr = (this->mpc.PC & 0xF8000000) + ((codeinMem & 0x03FFFFFF) << 1);
    int imme = (mcode&0xFFFF) << 16;
    imme >>= 16;
    this->ControlPanelStr = "";
    char buf[40];
    string cell;
    memset(buf,0,sizeof(char)*40);
    //instr
    sprintf(buf,"[instr]: %s\n",this->codeTbl[this->mpc.PC]);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //op
    sprintf(buf,"[op   ]: %d\n",opc);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //rs
    sprintf(buf,"[%5s]: %d\n",this->mpc.rgs->dRegWordTbl[rsnum],this->mpc.rgs->regContent[rsnum]);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //rt
    sprintf(buf,"[%5s]: %d\n",this->mpc.rgs->dRegWordTbl[rtnum],this->mpc.rgs->regContent[rtnum]);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //rd
    sprintf(buf,"[%5s]: %d\n",this->mpc.rgs->dRegWordTbl[rdnum],this->mpc.rgs->regContent[rdnum]);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //shmt
    sprintf(buf,"[shmt ]: %d\n",sanum);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //data
    sprintf(buf,"[data ]: [    %04x]%d\n",imme,imme);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //addr
    sprintf(buf,"[addr ]: [ %07x]%u\n",adr,adr);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
    //memory
    sprintf(buf,"[Memry]: [%08x]%u\n",codeinMem,codeinMem);
    cell = new string(buf);
    this->ControlPanelStr += cell;
    memset(buf,0,sizeof(char)*40);
}

void MainWindow::ExecuteProgram(string program, int choice);
void MainWindow::DebugProgram(string program, int choice);
