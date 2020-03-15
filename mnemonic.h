/**
** Author:		彭官妍 3160104633
** mnemonic:	助记符、格式指令与R/I/J/C/pseudo型指令集
** Func:		建立对应操作码查询表
*/

#ifndef _MNEMONIC_H_
#define _MNEMONIC_H_

#include <string>
#include <map>
using namespace std;

#define RINSNUM 29
#define IINSNUM 20
#define JINSNUM 2
#define PSEUDO  25
#define FINSNUM 9

typedef struct Mnemonics* TotalMs;
struct Mnemonics {
    string RtypeM[RINSNUM] = {
        "add","sub","slt","sltu","and","or","xor","nor",
        "sll","sllv","srl","srlv","sra","srav","jr","jalr",
        "syscall","mul","mult","multu","div","divu","mfhi",
        "mflo","mthi","mtlo","mfc0","mtc0","eret"
    };
    int RtypeOpc[RINSNUM] = { 0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,
                            0,28,0,0,0,0,0,0,0,0,16,16,16 };
    int RtypeFunc[RINSNUM] = { 32,34,42,43,36,37,38,39,
                            0,4,2,6,3,7,8,9,
                            12,2,24,25,26,27,16,18,17,19,0,0,24};
    string ItypeM[IINSNUM] = {
        "lui","addi","slti","sltiu","andi","ori","xori",
        "lw","lwx","lh","lhx","lhu","lhux","sw","swx","sh","shx",
        "beq","bne","bgezal"
    };
    int ItypeOpc[IINSNUM] = { 15,8,10,11,12,13,14,35,34,33,32,37,36,43,42,41,40,4,5,1 };
    string JtypeM[JINSNUM] = { "j","jal" };
    int JtypeOpc[JINSNUM] = { 2,3 };

    string pseudoM[PSEUDO] = {
        "push","pop","move","shi","shix",
        "inc","dec","addu","addiu","subu","not","neg",
        "abs","swap","b","beqz","bnez","beqi","bnei",
        "blt","ble","bgt","bge","seq","sne"
    };//将为伪指令编写替换指令，替换后再进行汇编

    //待完善的格式指令部分
    string insName[FINSNUM] = { "equ",".origin",".data",".text",".end",
        ".space",".zjie",".2zjie",".word" };

    map<string, int> RtypeToOpcTbl;
    map<string, int> RtypeToFuncTbl;
    map<string, int> ItypeToOpcTbl;
    map<string, int> JtypeToOpcTbl;
    map<string, int> PseudoTbl;
};

//初始化助记符相关函数
TotalMs initMnemonicTbls();
static void createRtypeOpcTbl(TotalMs* Tms);
static void createRtypeFuncTbl(TotalMs* Tms);
static void createItypeOpcTbl(TotalMs* Tms);
static void createJtypeOpcTbl(TotalMs* Tms);
static void createPseudoTbl(TotalMs* Tms);

#endif // !_MNEMONIC_H_
