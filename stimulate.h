#ifndef STIMULATE_H
#define STIMULATE_H
#include <map>
#include "register.h"
#include "mnemonic.h"
using namespace std;
typedef struct MIPSComputer Mipsc;
typedef struct MIPSComputer *MipscPtr;

//中断处理函数
void* print_int(void* a0, void* a1, void* a2);      //1
void* print_string(void* a0, void* a1, void* a2);   //4
void* read_int(void* a0, void* a1, void* a2);       //5
void* read_string(void* a0, void* a1, void* a2);    //8
void* sbrk(void* a0, void* a1, void* a2);           //9
void* exit(void* a0, void* a1, void* a2);           //10
void* print_char(void* a0, void* a1, void* a2);     //11
void* read_char(void* a0, void* a1, void* a2);      //12
void* open(void* a0, void* a1, void* a2);           //13
void* read(void* a0, void* a1, void* a2);           //14
void* write(void* a0, void* a1, void* a2);          //15
void* close(void* a0, void* a1, void* a2);          //16
void* exit2(void* a0, void* a1, void* a2);           //17

struct MIPSComputer{
    unsigned short MemoryMap[0x4000]; //内存以16位为一个字节,从0x3000开始是显存
    unsigned int PC;//程序计数器
    unsigned int dPC;//显存指针
    unsigned int dSegment;//数据段始址
    unsigned int cSegment;//程序段始址
    Regs rgs;       //寄存器（通用寄存器+协处理器0的部分寄存器）
    map<string, string> Macros;//宏，用于记录汇编代码定义的符号常量，同equTbl
    //函数指针数组（模拟syscall）
    void* (*syscallFuncPtr[18])(void* a0, void* a1, void* a2) = {
        NULL,   //占位0
        print_int,      //1
        NULL,    //占位2
        NULL,    //占位3
        print_string,   //4
        read_int,       //5
        NULL,     //占位6
        NULL,     //占位7
        read_string,    //8
        sbrk,           //9
        exit,           //10
        print_char,     //11
        read_char,      //12
        open,           //13
        read,           //14
        write,          //15
        close,          //16
        exit2           //17
    };
};

Mipsc buildComputer();
void mcbuildAssembler(Mipsc mpc, string program);
void mcbuildDisAssembler(Mipsc mpc, string program);
void ExecuteCode(Mipsc mpc);
void DebugCode(Mipsc mpc);
#endif // STIMULATE_H
