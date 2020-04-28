#ifndef STIMULATE_H
#define STIMULATE_H
#include <map>
#include <stdio.h>
#include <vector>
#include <bitset>
#include "register.h"
#include "mnemonic.h"
#include "assembler.h"
#include "disassembler.h"
#include "filehandler.h"

using namespace std;
#define MEMORYSIZE 0x4000
#define STACKSIZE  0x1000
#define DISPLAYVOLMUE 0x1000
typedef struct MIPSComputer Mipsc;
typedef struct MIPSComputer *MipscPtr;

//中断处理函数
//1
void* print_int(Mipsc &mpc, void* a0, void* a1, void* a2);
//11
void* print_char(Mipsc &mpc, void* a0, void* a1, void* a2);

struct MIPSComputer{
    unsigned short MemoryMap[MEMORYSIZE+STACKSIZE];   //内存以16位为一个字节,从0x3000开始是显存,从0x4000开始是堆栈
    unsigned int PC;//程序计数器
    unsigned int dPC;//显存指针
    unsigned int dSegment;//数据段始址
    unsigned int cSegment;//程序段始址
    unsigned int cEnd;    //程序段结束
    Regs rgs;       //寄存器（通用寄存器+协处理器0的部分寄存器）
    TotalMs ms;     //助记符
    map<string, string> Macros;//宏，用于记录汇编代码定义的符号常量，同equTbl
    //函数指针数组（模拟syscall）
    void* (*syscallFuncPtr[12])(Mipsc &mpc, void* a0, void* a1, void* a2) = {
        NULL,   //占位0
        NULL,      //1
        NULL,    //占位2
        NULL,    //占位3
        NULL,   //4
        NULL,       //5
        NULL,     //占位6
        NULL,     //占位7
        NULL,    //8
        NULL,           //9
        NULL,           //10
        print_char,     //11
    };
    int isDebugBegin;
    unsigned int lastPC;
    int intRequest;//中断值
    void* intRetValuePtr;
    int intTriggled;
};

Mipsc buildComputer();
void resetRegisterFile(MipscPtr Pmpc);
void resetPC(MipscPtr Pmpc);
void mcbuildAssembler(MipscPtr Pmpc, string program);
void mcbuildDisAssembler(MipscPtr Pmpc, string program);
void ExecuteCode(Mipsc &mpc, string program, int strType);
void DebugCode(Mipsc &mpc, string program, int strType);
void emptyDisplayMemory(Mipsc &mpc);
static void setWelcomeStr(Mipsc &mpc);
static void _execCode(Mipsc &mpc, int execmode);

#endif // STIMULATE_H
