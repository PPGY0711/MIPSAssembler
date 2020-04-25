#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H
#include <vector>
#include <map>
#include <string>
using namespace std;

/*
typedef struct Instruction Inst;
typedef struct Instruction *InstPtr;
struct Instruction{
    string strstat;
    int addr;
};

typedef struct InstructionSet InstSet;
typedef struct InstructionSet *InstSetPtr;
struct InstructionSet{
    InstPtr ISet;
    int size;
};
*/
//反汇编器接口函数，接收机器码字符串
string disassembler(string machinecode);
//void disassembler(string program, unsigned short MemoryMap[]);
//解析一行机器码
string parseStatement(string bincode, int lineNum);
//指令翻译相关函数
static string RmcTranslate(string code, string mnemonic);
static string CmcTranslate(string code, string mnemonic);
static string ImcTranslate(string code, string mnemonic, int lineNum);
static string JmcTranslate(string code, string mnemonic, int lineNum);
#endif // DISASSEMBLER_H
