#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <fstream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <math.h>
#include <string.h>
using namespace std;
//1.读入汇编文件
string readInputFile(string filename);
//3.消除注释
void getRidOfNotes(string& code);
//4.按照分隔符delim分割字符串，处理汇编代码字符流用
vector<string> splitC(string s, char delim);
//5.消除语句中空格、回车、制表符符号
string trim(string& str);
//6.将二进制字符串转化为十六进制字符串
string StrToHex(const string str);
//7.计算某字符串中某字符出现的总次数
unsigned int c_in_str(const char * str, char ch);
//8.字符串与数字转换相关
//二进制字符串转化成十进制数
unsigned int ubinStr2dex(string ubs);
int binStr2dex(string bs);
string dex2str(int num);
string udex2str(unsigned int num);
#endif
