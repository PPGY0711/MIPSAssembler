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

    string mnemonics[IINSNUM+JINSNUM+3] = {
        "lui","addi","slti","sltiu","andi","ori","xori",
        "lw","lwx","lh","lhx","lhu","lhux","sw","swx","sh","shx",
        "beq","bne","bgezal",
        "j","jal",
        "R1","R2","R3"
    };
    int opcs[IINSNUM+JINSNUM+3] = {
        15,8,10,11,12,13,14,
        35,34,33,32,37,36,43,42,41,40,
        4,5,1,
        2,3,
        0,28,16
    };

    string r1s[RINSNUM-4] = {
        "add","sub","slt","sltu","and","or","xor","nor",
        "sll","sllv","srl","srlv","sra","srav","jr","jalr",
        "syscall","mult","multu","div","divu","mfhi",
        "mflo","mthi","mtlo"
    };
    int r1sFuncs[RINSNUM-4] = {
        32,34,42,43,36,37,38,39,
        0,4,2,6,3,7,8,9,
        12,24,25,26,27,16,18,17,19
    };
    string r2s[1] = {"mul"};
    int r2sFuncs[1] = {2};
    string r3s[3] = {"mfc0","mtc0","eret"};
    int r3sRs[3] = {0,4,16};
    map<string, int> RtypeToOpcTbl;
    map<string, int> RtypeToFuncTbl;
    map<string, int> ItypeToOpcTbl;
    map<string, int> JtypeToOpcTbl;
    map<string, int> PseudoTbl;
    map<int, string> Opc2MnemonicTbl;
    map<int, string> r1Func2R1Tbl;
    map<int, string> r2Func2R2Tbl;
    map<int, string> r3Rs2R3Tbl;

    string allmnemonics[RINSNUM+IINSNUM+JINSNUM] = {
        "add","sub","slt","sltu","and","or","xor","nor",
        "sll","sllv","srl","srlv","sra","srav","jr","jalr",
        "syscall","mul","mult","multu","div","divu","mfhi",
        "mflo","mthi","mtlo","mfc0","mtc0","eret",
        "lui","addi","slti","sltiu","andi","ori","xori",
        "lw","lwx","lh","lhx","lhu","lhux","sw","swx","sh","shx",
        "beq","bne","bgezal",
        "j","jal"
    };
    map<string, int> mne2codeTbl;
};

//初始化助记符相关函数
TotalMs initMnemonicTbls();
string getMnemonic(TotalMs Tms, int opc, int func, int r3num, int* type);
static void createRtypeOpcTbl(TotalMs* Tms);
static void createRtypeFuncTbl(TotalMs* Tms);
static void createItypeOpcTbl(TotalMs* Tms);
static void createJtypeOpcTbl(TotalMs* Tms);
static void createPseudoTbl(TotalMs* Tms);
static void createOpc2MneTbl(TotalMs* Tms);
static void createR1Func2R1Tbl(TotalMs* Tms);
static void createR2Func2R2Tbl(TotalMs* Tms);
static void createR3Rs2R3Tbl(TotalMs* Tms);
static void createMne2CodeTbl(TotalMs* Tms);
#endif // !_MNEMONIC_H_
