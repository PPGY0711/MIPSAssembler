/**
** Author:		彭官妍 3160104633
** Assembler:	汇编器实现主要逻辑，进行代码二次扫描并翻译成机器码保存
** Func:		读入汇编代码，整理成标准格式并翻译，将机器码以文件形式保存
*/

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_
#include <vector>
#include <string>
#include <map>
using namespace std;

//定义汇编代码处理数据结构、机器码生成数据结构
typedef struct AsmCodeRecord	*ACRec;
typedef struct MachineCodeSet	*MCSet;
struct AsmCodeRecord {
    map<int, string> CodeTbl;
    map<string, int> LTbl;//int是Label所在代码行数
    map<int, string> LETbl;//记录该行是否存在标号的表
//    map<string, int> DTbl;//记录数据定义的表（暂未实现）
};
struct MachineCodeSet {
    map<int, string> MCodeTbl;
    map<int, unsigned int> AddrTbl;
};
//第一次扫描代码
void firstScan(string asmcode, ACRec* asmC);
//计算语句中算术表达式的值并返回该值
static int calculateExp(string exp);
//第一次扫描中添加标号到标号表中
static int insertLabel(string s, int linenum, map<string, int>& tbl);
//处理伪指令，对本程序支持的部分伪指令进行替换
static string repPseudoWithGeneral(string s);
//计算某运算符的优先级，处理算术表达式用
static int priority(int state, char a);
//计算部分算术表达式的值
static double calculate(char op, double op1, double op2);
//返回整个算术表达式的值
static int calculateExp(string exp);
//第二次扫描代码（生成机器码及地址）
void genMachineCode(ACRec* asmC, MCSet* mcSet);
//指令翻译相关函数
static string RtTranslate(string code, int linenum,MCSet* mcSet, ACRec* asmC);
static string ItTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC);
static string JtTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC);
//生成R/I/J型指令机器码内部接口
static string genRMcode(int opc,int rsnum,int rtnum,int rdnum, int sa,int func);
static string genIMcode(int opc,int rsnum,int rtnum,int simme,int usimme,int offset, int choice);
//返回程序段的机器码的外部接口（结果或者错误信息）
string printMachineCode(MCSet* mcSet,int FileType);
string printMachineCode();
//UI调用Assembler的外部接口
bool assembler(string filename, string content, int choice, string &result, int FileType);
#endif // !_TXTFORMAT_H_
