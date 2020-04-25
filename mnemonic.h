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
#define RINSNUM 26
#define IINSNUM 20
#define JINSNUM 2
#define CINSNUM 3
#define PSEUDO  27
#define FINSNUM 9

typedef struct Mnemonics* TotalMs;
struct Mnemonics {
    string RtypeM[RINSNUM] = {
        "add","sub","slt","sltu","and","or","xor","nor",
        "sll","sllv","srl","srlv","sra","srav","jr","jalr",
        "syscall","mul","mult","multu","div","divu","mfhi",
        "mflo","mthi","mtlo"
    };
    int RtypeOpc[RINSNUM] = { 0,0,0,0,0,0,0,0,
                            0,0,0,0,0,0,0,0,
                            0,28,0,0,0,0,0,0,0,0};
    int RtypeFunc[RINSNUM] = { 32,34,42,43,36,37,38,39,
                            0,4,2,6,3,7,8,9,
                            12,2,24,25,26,27,16,18,17,19};

    string CtypeM[CINSNUM] = {"mfc0","mtc0","eret"};
    int CtypeOpc[CINSNUM] = {16,16,16};
    int CtypeFunc[CINSNUM] = {0,0,24};

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
        "blt","ble","bgt","bge","seq","sne","li","la"
    };//将为伪指令编写替换指令，替换后再进行汇编

    string mnemonics[IINSNUM+JINSNUM+3] = {
        "lui","addi","slti","sltiu","andi","ori","xori",
        "lw","lwx","lh","lhx","lhu","lhux","sw","swx","sh","shx",
        "beq","bne","bgezal",
        "j","jal",
        "mul","R","C"
    };
    int opcs[IINSNUM+JINSNUM+3] = {
        15,8,10,11,12,13,14,
        35,34,33,32,37,36,43,42,41,40,
        4,5,1,
        2,3,
        28,0,16
    };

    string rmnes[RINSNUM - 1] = {
        "add","sub","slt","sltu","and","or","xor","nor",
        "sll","sllv","srl","srlv","sra","srav","jr","jalr",
        "syscall","mult","multu","div","divu","mfhi",
        "mflo","mthi","mtlo"
    };
    int rmnesFuncs[RINSNUM-1] = {
        32,34,42,43,36,37,38,39,
        0,4,2,6,3,7,8,9,
        12,24,25,26,27,16,18,17,19
    };

    //待完善的格式指令部分
    string insName[FINSNUM] = { "equ",".origin",".data",".text",".end",
        ".space",".zjie",".2zjie",".word" };

    string cmnes[3] = {"mfc0","mtc0","eret"};
    int cmnesRs[3] = {0,4,16};
    map<string, int> RtypeToOpcTbl;
    map<string, int> RtypeToFuncTbl;
    map<string, int> CtypeToOpcTbl;
    map<string, int> CtypeToFuncTbl;
    map<string, int> ItypeToOpcTbl;
    map<string, int> JtypeToOpcTbl;
    map<string, int> PseudoTbl;
    map<int, string> Opc2MnemonicTbl;
    map<int, string> r1Func2R1Tbl;
    map<int, string> crs2mneTbl;

    string allmnemonics[RINSNUM+IINSNUM+JINSNUM+CINSNUM] = {
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
    map<string, int> formatInstTbl;
};

//初始化助记符相关函数
TotalMs initMnemonicTbls();
string getMnemonic(TotalMs Tms, int opc, int func, int rsnum, int* type);
static void createRtypeOpcTbl(TotalMs* Tms);
static void createRtypeFuncTbl(TotalMs* Tms);
static void createCtypeOpcTbl(TotalMs* Tms);
static void createCtypeFuncTbl(TotalMs* Tms);
static void createItypeOpcTbl(TotalMs* Tms);
static void createJtypeOpcTbl(TotalMs* Tms);
static void createPseudoTbl(TotalMs* Tms);
static void createOpc2MneTbl(TotalMs* Tms);
static void createR1Func2R1Tbl(TotalMs* Tms);
static void createCRs2CMneTbl(TotalMs* Tms);
static void createMne2CodeTbl(TotalMs* Tms);
static void createFormatInstTbl(TotalMs* Tms);
#endif // !_MNEMONIC_H_
