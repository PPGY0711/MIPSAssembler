/**
** Author:		彭官妍 3160104633
** registers:	包含通用寄存器名与对应编号及存储值、协处理器0寄存器与编号与存储值
** Func:		建立对应名称查询表
*/
#ifndef _REGISTER_H_
#define _REGISTER_H_

#include <string>
#include <map>
using namespace std;
#define CPURNUM 32
#define COPRORNUM 25
#define NOTEXIST -1
typedef struct Registers* Regs;
struct Registers
{
    string regWordName[CPURNUM] = {
        "$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0",
        "$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0",
        "$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8",
        "$t9","$k0","$k1","$gp","$sp","$fp","$ra" };
    string regNumName[CPURNUM] = {
        "$0","$1","$2","$3","$4","$5","$6","$7","$8",
        "$9","$10","$11","$12","$13","$14","$15","$16",
        "$17","$18","$19","$20","$21","$22","$23","$24",
        "$25","$26","$27","$28","$29","$30","$31" };
    //待完善的协处理器部分
    string coprocessor[COPRORNUM] = {
        "Index","Random","EntryLo0","EntryLo1","Context",
        "PageMask","Wired","BadVAddr","Count","EntryHi",
        "Compare","Status","Cause","Cause","EPC","PRId",
        "Config","WatchLo","WatchHi","Debug","DEPC","ErrCtl",
        "TagLo","ErrorEPC","DESAVE"
    };
    string regContent[CPURNUM] = { "0" };
    string coproContent[COPRORNUM] = { "0" };

    string hiContent = { "0" };
    string loContent = { "0" };

    int regNum[CPURNUM] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
                        20,21,22,23,24,25,26,27,28,29,30,31 };
    int regNumCo[COPRORNUM] = { 0,1,2,3,4,5,6,8,9,10,11,12,13,14,15,16,17,18,19,
                        23,24,26,28,30,31 };

    map<string, int> regWordTbl;
    map<string, int> regNumTbl;
    map<string, int> regCoWordTbl;
};

//初始化寄存器相关函数
Regs initRegTbls();
int getRegisterNum(string name, Regs Rs);
static void createRegCoWordTbl(Regs* Rs);
static void createRegWordTbl(Regs* Rs);
static void createRegNumTbl(Regs* Rs);
#endif // !_REGISTERS_H_
