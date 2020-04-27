#pragma warning(disable:4996)
#include "assembler.h"
#include "filehandler.h"
#include "stimulate.h"
#include <fstream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <bitset>
#include <stack>
#include <math.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <regex>
#define ERROR -99999
#define DEFAULTORIGIN 0x0000
string ErrorMsg = "No Error.\n";
Regs rgs;
TotalMs ms;
unsigned int curOrigin;
unsigned short MMap[0x3000];
unsigned int PC;
enum variableType{NUM=0,STR,CHAR};
static int isPseudo(string s)
{
    //如果为部分格式指令，直接返回
    if(s.find('.')!=string::npos)
        return 0;
    string tmpm;
    if (s.find('$') != string::npos)
        if (s.find(':') == string::npos)
            tmpm = (s.substr(0, s.find_first_of('$')));
        else
            tmpm = (s.substr(s.find_first_of(':') + 1, s.find_first_of('$')-s.find_first_of(':')-1));
    else if (s.find("b ") != string::npos)
        tmpm = "b";
    tmpm = trim(tmpm);
//    cout<< "isPseudo: " << tmpm <<endl;
    if (ms->PseudoTbl.find(tmpm) == ms->PseudoTbl.end())
        return 0;
    else
        return 1;
}

static string repPseudoWithGeneral(string s)
{
    string mne, regs, bstr, repstr,newstr, regstr = "";
    int nreg;
    char* tmp = new char[16];
    char* ss = new char[16];
    vector<string> regArray;
    int which;
    string label = "";
    if(s.find(':') !=string::npos)
        label = s.substr(0,s.find_first_of(':'));
    label = trim(label);
    if (s.find('$') != string::npos)
        if (s.find(':') == string::npos)
            mne = (s.substr(0, s.find_first_of('$')));
        else
            mne = (s.substr(s.find_first_of(':') + 1, s.find_first_of('$')));
    else if (s.find("b ") != string::npos) {
        mne = "b";
        bstr = "beq $zero,$zero," + s.substr(s.find("b") + 1) + "\n";
    }
    mne = trim(mne);
    regs = s.substr(s.find_first_of('$'));
    regs = trim(regs);
    which = ms->PseudoTbl[mne];
    regArray = splitC(regs, ',');
    int imme;
    switch (which)
    {
    case 0://"push"
    case 1://"pop"
        nreg = c_in_str(s.c_str(), ',') + 1;
        for (int i = 0; i < nreg; i++)
        {
            string tmpstr;
            if (which == 0)
                tmpstr = "sw ";
            else
                tmpstr = "lw ";
            tmpstr += regArray[i];
            tmpstr += ",";
            tmpstr += itoa(i * 2, ss, 10);
            tmpstr += "($sp)\n";
            regstr += tmpstr;
        }
        if (which == 0) {
            repstr.assign(itoa(-(nreg * 2), tmp, 10));
            newstr = "addi $sp,$sp," + repstr + "\n";
            newstr += regstr;
        }
        else
        {
            repstr.assign(itoa((nreg * 2), tmp, 10));
            newstr = "addi $sp,$sp," + repstr + "\n";
            newstr = regstr + newstr;
        }
        break;
    case 2://"move"
        newstr = "or " + s.substr(s.find_first_of('$')) + ",$zero\n";
        break;
    case 3://"shi"
    case 4://"shix"
        newstr = "addi $at,$zero," + s.substr(s.find_first_of('$'), s.find_first_of(',') - s.find_first_of('$')) + "\n";
        if(which == 3)
            newstr += "sh $at," + s.substr(s.find_first_of(',') + 1) + "\n";
        else
            newstr += "shx $at," + s.substr(s.find_first_of(',') + 1) + "\n";
        break;
    case 5://"inc"
        newstr = "addi " + trim(regs) + "," + trim(regs) + ",1\n";
        break;
    case 6://"dec"
        newstr = "addi " + trim(regs) + "," + trim(regs) + ",-1\n";
        break;
    case 7://"addu"
        newstr = "add " + regs + "\n";
        break;
    case 8://"addiu"
        newstr = "addi " + regs + "\n";
        break;
    case 9://"subu"
        newstr = "sub " + regs + "\n";
        break;
    case 10://"not"
        newstr = "nor " + regs + "," + regs.substr(regs.find(',') + 1) + "\n";
        break;
    case 11://"neg"
        newstr = "sub " + regs.substr(0, regs.find(',')) + ",$zero," + regs.substr(regs.find(',') + 1) + "\n";
        break;
    case 12://"abs"
        newstr = "sra $at," + regArray[0] + ",31\n";
        newstr += "xor " + regs + ",$at\n";
        newstr += "sub " + regArray[0] + "," + regArray[0] + ",$at\n";
        break;
    case 13://"swap"
        newstr = "xor " + regArray[0] + "," + regArray[0] + "," + regArray[1] + "\n";
        newstr += "xor " + regArray[1] + "," + regArray[0] + "," + regArray[1] + "\n";
        newstr += "xor " + regArray[0] + "," + regArray[0] + "," + regArray[1] + "\n";
        break;
    case 14://"b"
        newstr = bstr;
        break;
    case 15://"beqz"
        newstr = "beq " + regArray[0] + ",$zero," + regArray[1] + "\n";
        break;
    case 16://"bnez"
        newstr = "bne " + regArray[0] + ",$zero," + regArray[1] + "\n";
        break;
    case 17://"beqi"
        newstr = "addi $at,$zero," + regArray[1] + "\n";
        newstr += "beq $at," + regArray[0] + "," + regArray[2] + "\n";
        break;
    case 18://"bnei"
        newstr = "addi $at,$zero," + regArray[1] + "\n";
        newstr += "bne $at," + regArray[0] + "," + regArray[2] + "\n";
        break;
    case 19://"blt"
        newstr = "slt $at," + regArray[0] + "," + regArray[1]+ "\n";
        newstr += "bne $at,$zero," + regArray[2] + "\n";
        break;
    case 20://"ble"
        newstr = "slt $at," + regArray[1] + "," + regArray[0] + "\n";
        newstr += "beq $at,$zero," + regArray[2] + "\n";
        break;
    case 21://"bgt"
        newstr = "slt $at," + regArray[1] + "," + regArray[0] + "\n";
        newstr += "bne $at,$zero," + regArray[2] + "\n";
        break;
    case 22://"bge"
        newstr = "slt $at," + regArray[0] + "," + regArray[1] + "\n";
        newstr += "beq $at,$zero," + regArray[2] + "\n";
        break;
    case 23://"seq"
        regArray = splitC(regs, ',');
        newstr = "sub " + regArray[0] + "," + regArray[1] + "," + regArray[2] + "\n";
        newstr += "sltiu " + regArray[0] + "," + regArray[1] + ",1\n";
        break;
    case 24://"sne"
        newstr = "sub " + regArray[0] + "," + regArray[1] + "," + regArray[2] + "\n";
        newstr += "sltu " + regArray[0] + ",$zero," + regArray[0] + "\n";
        break;
    case 25://"li"
    case 26://"la"
        if(isWithinLimit(regArray[1])){
            newstr = "addi " + regArray[0] + ",$zero," + regArray[1];
        }
        else{
            newstr = "lui " + regArray[0] +", HIGH " +regArray[1] +"\n";
            newstr += "ori " + regArray[0] +", LOW " +regArray[1] +"\n";
        }
        break;
    }
    newstr = label + ": " + newstr;
    return newstr;
}

//1.第一次扫描代码（先消除注释，再消除一些格式指令/伪指令），化成每行一个指令代码，标号与其后的代码同一行
void firstScan(string asmcode, ACRec* asmC)
{
    //消除注释
    getRidOfNotes(asmcode);
    int tmpid = 0;
    //替换伪指令
    vector<string> tmpcodePerLine = splitC(asmcode, '\n');
    vector<string> codePerLine;
    for (int i = 0; i < tmpcodePerLine.size(); i++) {
        if (isPseudo(tmpcodePerLine[i])) {
            vector<string> repstr = splitC(repPseudoWithGeneral(tmpcodePerLine[i]), '\n');
            for (int j = 0; j < repstr.size(); j++) {
                codePerLine.push_back(repstr[j]);
            }
        }
        //这一步加到汇编代码中的还有格式指令
        else if(!(isEmpty(tmpcodePerLine[i]))){
            codePerLine.push_back(tmpcodePerLine[i]);
        }
    }
    //处理标号
    for (int i = 0; i < codePerLine.size(); i++) {
        cout<< "Line[" << i << "]:" << codePerLine[i] <<endl;
        if(isFormatCode(codePerLine[i])){
            //根据格式指令的语义进行预处理（调整基地址，记录偏移）
            ParseFormatCode(codePerLine[i],asmC);
            (*asmC)->CodeTbl.insert(pair<int,string>(i,codePerLine[i]));
        }
        else{            
            if ((tmpid = codePerLine[i].find(':')) != string::npos)
            {
                if (!insertLabel(codePerLine[i], i, (*asmC)->LTbl)) {
                    ErrorMsg.assign("Error: One Label Cannot Be Used Twice!\n");
                    //错误信息监控
                }
                else {
                    string ts = codePerLine[i].substr(codePerLine[i].find_first_of(':') + 1);
                    string tl = codePerLine[i].substr(0,codePerLine[i].find(':'));
                    (*asmC)->CodeTbl.insert(pair<int, string>(i, ts));
                    (*asmC)->LETbl.insert(pair<int, string>(i, trim(tl)));
                }
            }
            else {
                (*asmC)->CodeTbl.insert(pair<int, string>(i, codePerLine[i]));
                (*asmC)->LETbl.insert(pair<int, string>(i, "-"));
            }
            (*asmC)->asmAddrTbl.insert(pair<int, unsigned int>(i,PC));
            PC = PC + 2;
        }
    }
}

static int insertLabel(string s, int linenum, map<string, int>& tbl)
{
    string tmpL = s.substr(0, s.find_first_of(':'));
    trim(tmpL);
    if (tbl.find(tmpL) == tbl.end()) {
        cout<< "insert label: " << tmpL << " at Line[" << linenum <<"]"<<endl;
        tbl.insert(pair<string, int>(tmpL, linenum));
        return 1;
    }
    else
        return 0;
}

//2.第二次扫描代码（生成机器码及地址）
void genMachineCode(ACRec* asmC, MCSet* mcSet)
{
    //第二次扫描开始前，把有标号的行的地址先填到AddrTbl表中
    map<string, int>::iterator tmpit;
    tmpit = (*asmC)->LTbl.begin();
    while(tmpit!=(*asmC)->LTbl.end()){
        unsigned int laddr = (*asmC)->asmAddrTbl[tmpit->second];
        (*mcSet)->AddrTbl.insert(pair<int, unsigned int>(tmpit->second, laddr));
        cout << tmpit->first << " " << laddr <<endl;
        tmpit++;
    }
    unsigned int lineaddr;
    map<int, string>::iterator iter;
    iter = (*asmC)->CodeTbl.begin();
    while (iter != (*asmC)->CodeTbl.end()) {
        //1.生成每行指令的"绝对"地址（初始地址加上偏移）
        lineaddr = (*asmC)->asmAddrTbl[iter->first];
        (*mcSet)->AddrTbl.insert(pair<int, unsigned int>(iter->first, lineaddr));
        cout << "insert Addr at Line[" <<iter->first <<"] :" <<  lineaddr <<endl;
        //2.翻译指令
        if(!isFormatCode(iter->second)){
            string tmpmenomic, machineCode;
            //不带寄存器符号的eret/syscall
            if (iter->second.find('$') == string::npos) {
                string tmps = iter->second;
                tmps = trim(tmps);
                if (tmps[0] == 'j') {
                    machineCode = JtTranslate(iter->second, iter->first, mcSet, asmC);
                    (*mcSet)->MCodeTbl.insert(pair<int, string>(iter->first, machineCode));
                }
                else if(tmps[0] == 'e'){
                    machineCode = CtTranslate(iter->second, iter->first, mcSet, asmC);
                    (*mcSet)->MCodeTbl.insert(pair<int, string>(iter->first, machineCode));
                }
                else{
                    machineCode = RtTranslate(iter->second, iter->first, mcSet, asmC);
                    (*mcSet)->MCodeTbl.insert(pair<int, string>(iter->first, machineCode));
                }
            }
            else {
                tmpmenomic = iter->second.substr(0, iter->second.find_first_of('$'));
                tmpmenomic = trim(tmpmenomic);
                if (ms->RtypeToOpcTbl.find(tmpmenomic) != ms->RtypeToOpcTbl.end())
                    machineCode = RtTranslate(iter->second, iter->first, mcSet, asmC);
                else if (ms->ItypeToOpcTbl.find(tmpmenomic) != ms->ItypeToOpcTbl.end())
                    machineCode = ItTranslate(iter->second, iter->first, mcSet, asmC);
                else if(ms->CtypeToOpcTbl.find(tmpmenomic) != ms->CtypeToOpcTbl.end())
                    machineCode = CtTranslate(iter->second, iter->first, mcSet, asmC);
                else
                    ErrorMsg.assign("Error: Undefined mnemonic used!\n");
                (*mcSet)->MCodeTbl.insert(pair<int, string>(iter->first, machineCode));
            }
            (*asmC)->endAddr = lineaddr;
            insertMCodeToMemory(lineaddr,machineCode);
        }
        iter++;
    }
}

//考虑通用的32个寄存器
static string RtTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC)
{
    string tmpC, mcode;
    tmpC = code;
    //"" eret/syscall
    if (tmpC.find('$') == string::npos) {
        if (tmpC.find("syscall") != string::npos) {
            tmpC = trim(tmpC);
            if (tmpC.compare("syscall") == 0)
                return "00000000000000000000000000001100";
        }
    }
    else {
        string mne, rs, rt, rd, saExp, regs;
        int func, opc, sa, rsnum, rtnum, rdnum;
        mne = tmpC.substr(0, tmpC.find_first_of('$'));
        mne = trim(mne);
        regs = tmpC.substr(tmpC.find_first_of('$'));
        regs = trim(regs);
        opc = ms->RtypeToOpcTbl[mne];
        func = ms->RtypeToFuncTbl[mne];
        switch (opc)
        {
        case 0:		//arithmatic类型
            switch (func)
            {
                    //rd,rs,rt
            case 32:
            case 34:
            case 42:
            case 43:
            case 36:
            case 37:
            case 38:
            case 39:
            case 7:
            case 4:
            case 6:
                rd = regs.substr(0, regs.find_first_of(','));
                rdnum = getRegisterNum(rd, rgs);
                rs = regs.substr(regs.find_first_of(',') + 1, regs.find_last_of(',') - regs.find_first_of(',') - 1);
                rsnum = getRegisterNum(rs, rgs);
                rt = regs.substr(regs.find_last_of(',') + 1);
                rtnum = getRegisterNum(rt, rgs);
                if ((rdnum != NOTEXIST) && (rsnum != NOTEXIST) && (rtnum != NOTEXIST))
                    mcode = genRMcode(opc, rsnum, rtnum, rdnum, 0, func);
                break;
                    //rd,rs,sa
            case 0:
            case 2:
            case 3:
                rd = regs.substr(0, regs.find_first_of(','));
                rdnum = getRegisterNum(rd, rgs);
                rs = regs.substr(regs.find_first_of(',') + 1, regs.find_last_of(',') - regs.find_first_of(',') - 1);
                rsnum = getRegisterNum(rs, rgs);
                saExp = regs.substr(regs.find_last_of(',') + 1);
                sa = calculateExp(saExp, asmC);
                if ((rdnum != NOTEXIST) && (rsnum != NOTEXIST))
                    mcode = genRMcode(opc, rsnum, 0, rdnum, sa, func);
                break;
                    //rs
            case 8:
            case 17:
            case 19:
                rs = regs;
                rsnum = getRegisterNum(rs, rgs);
                if ((rsnum != NOTEXIST))
                    mcode = genRMcode(opc, rsnum, 0, 0, 0, func);
                break;
                    //rs,rd
            case 9:
                rs = regs.substr(0, regs.find_first_of(','));
                rsnum = getRegisterNum(rs, rgs);
                rd = regs.substr(regs.find_first_of(',') + 1);
                rdnum = getRegisterNum(rd, rgs);
                if ((rdnum != NOTEXIST) && (rsnum != NOTEXIST))
                    mcode = genRMcode(opc, rsnum, 0, rdnum, 0, func);
                break;
                    //rs,rt
            case 24:
            case 25:
            case 26:
            case 27:
                rs = regs.substr(0, regs.find_first_of(','));
                rsnum = getRegisterNum(rs, rgs);
                rt = regs.substr(regs.find_first_of(',') + 1);
                rtnum = getRegisterNum(rt, rgs);
                if ((rtnum != NOTEXIST) && (rsnum != NOTEXIST))
                    mcode = genRMcode(opc, rsnum, rtnum, 0, 0, func);
                break;
                    //rd
            case 16:
            case 18:
                rd = regs;
                rdnum = getRegisterNum(rd, rgs);
                if ((rdnum != NOTEXIST))
                    mcode = genRMcode(opc, 0, 0, rdnum, 0, func);
                break;
            }
            break;
        case 28:	//mul
            rd = regs.substr(0, regs.find_first_of(','));
            rdnum = getRegisterNum(rd, rgs);
            rs = regs.substr(regs.find_first_of(',') + 1, regs.find_last_of(',') - regs.find_first_of(',') - 1);
            rsnum = getRegisterNum(rs, rgs);
            rt = regs.substr(regs.find_last_of(',') + 1);
            rtnum = getRegisterNum(rt, rgs);
            if ((rdnum != NOTEXIST) && (rsnum != NOTEXIST) && (rtnum != NOTEXIST))
                mcode = genRMcode(opc, rsnum, rtnum, rdnum, 0, func);
            break;
        }
    }
    return mcode;
}

static string genRMcode(int opc, int rsnum, int rtnum, int rdnum, int sa, int func)
{
    string str = "";
    bitset<6> opcb(opc);
    bitset<5> rsc(rsnum), rtc(rtnum), rdc(rdnum), sac(sa);
    bitset<6> funcc(func);
    str += opcb.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rsc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rtc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rdc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += sac.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += funcc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    return str;
}

//考虑协处理器
static string CtTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC)
{
    string tmpC, mcode = "";
    tmpC = code;
    //"" eret
    if (tmpC.find('$') == string::npos) {
        if ((tmpC.find("eret") != string::npos)) {
            tmpC = trim(tmpC);
            if (tmpC.compare("eret") == 0)
                return "01000010000000000000000000011000";
        }
    }
    else {
        string mne, rt, rc, regs;
        int func, opc, rtnum, rcnum;
        mne = tmpC.substr(0, tmpC.find_first_of('$'));
        mne = trim(mne);
        regs = tmpC.substr(tmpC.find_first_of('$'));
        regs = trim(regs);
        opc = ms->CtypeToOpcTbl[mne];
        func = ms->CtypeToFuncTbl[mne];
        rt = regs.substr(0, regs.find_first_of(','));
        rtnum = getRegisterNum(rt, rgs);
        rc = regs.substr(regs.find_first_of(',') + 1);
        rcnum = getRegisterNum(rc, rgs);
        cout << rtnum << ": " << rt << " " << rcnum<< ": " <<rc << endl;
        switch (mne[1]) {
        case 'f':
            if ((rtnum != NOTEXIST) && (rcnum != NOTEXIST))
                mcode = genCMcode(opc, 0, rtnum, rcnum, 0, func);
            break;
        case 't':
            if ((rtnum != NOTEXIST) && (rcnum != NOTEXIST))
                mcode = genCMcode(opc, 4, rtnum, rcnum, 0, func);
            break;
        default:
            break;
        }
    }
    cout << "C type mcode: " <<mcode <<endl;
    return mcode;
}

static string genCMcode(int opc, int rsnum, int rtnum, int rdnum, int sa, int func)
{
    string str = "";
    bitset<6> opcb(opc);
    bitset<5> rsc(rsnum), rtc(rtnum), rdc(rdnum), sac(sa);
    bitset<6> funcc(func);
    str += opcb.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rsc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rtc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rdc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += sac.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += funcc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    cout << "generate c type str: " << str <<endl;
    return str;
}

static string ItTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC)
{
    string tmpC, mcode = "";
    tmpC = code;
    string mne, rs, rt,  off_imme, label, regs;
    int opc, rsnum, rtnum, off_imme_num;
    unsigned int uimme,lineaddr,labeladdr;
    mne = tmpC.substr(0, tmpC.find_first_of('$'));
    mne = trim(mne);
    regs = tmpC.substr(tmpC.find_first_of('$'));
    regs = trim(regs);
    opc = ms->ItypeToOpcTbl[mne];
    switch (opc)
    {
            //rt,dat->lui
    case 15:
        rt = regs.substr(0, regs.find_first_of(','));
        rtnum = getRegisterNum(rt, rgs);
        off_imme = regs.substr(regs.find_first_of(',') + 1);
        off_imme_num = calculateExp(off_imme, asmC);
        mcode = genIMcode(opc, 0, rtnum, off_imme_num, 0, 0, 1);
        break;
            //rt,rs,dat
    case 8:   
    case 10:
        rt = regs.substr(0, regs.find_first_of(','));
        rtnum = getRegisterNum(rt, rgs);
        rs = regs.substr(regs.find_first_of(',') + 1, regs.find_last_of(',') - regs.find_first_of(',') - 1);
        rsnum = getRegisterNum(rs, rgs);
        off_imme = regs.substr(regs.find_last_of(',') + 1);
        off_imme_num = calculateExp(off_imme, asmC);
        mcode = genIMcode(opc, rsnum, rtnum, off_imme_num, 0, 0, 1);
        break;
        //rt,rs,dot
    case 11:
    case 12:
    case 13:
    case 14:
        rt = regs.substr(0, regs.find_first_of(','));
        rtnum = getRegisterNum(rt, rgs);
        rs = regs.substr(regs.find_first_of(',') + 1, regs.find_last_of(',') - regs.find_first_of(',') - 1);
        rsnum = getRegisterNum(rs, rgs);
        off_imme = regs.substr(regs.find_last_of(',') + 1);
        off_imme_num = calculateExp(off_imme, asmC);
        uimme = off_imme_num & 0xFFFFFFFF;
        mcode = genIMcode(opc, rsnum, rtnum, 0, uimme, 0, 2);
        break;
            //rt,dat(rs)
    case 35:
    case 34:
    case 33:
    case 32:
    case 37:
    case 36:
    case 43:
    case 42:
    case 41:
    case 40:
        rt = regs.substr(0, regs.find_first_of(','));
        rtnum = getRegisterNum(rt, rgs);
        rs = regs.substr(regs.find_first_of('(') + 1, regs.find_last_of(')') - regs.find_last_of('(') - 1);
        rsnum = getRegisterNum(rs, rgs);
        off_imme = regs.substr(regs.find_last_of(',') + 1,regs.find_last_of('(')-regs.find_first_of(',') - 1);
        off_imme_num = calculateExp(off_imme, asmC);
        mcode = genIMcode(opc, rsnum, rtnum, off_imme_num, 0, 0, 1);
        break;
            //rs,rt,ofs
    case 4:
    case 5:
        rs = regs.substr(0, regs.find_first_of(','));
        rsnum = getRegisterNum(rs, rgs);
        rt = regs.substr(regs.find_first_of(',') + 1, regs.find_last_of(',') - regs.find_first_of(',') - 1);
        rtnum = getRegisterNum(rt, rgs);
        label = regs.substr(regs.find_last_of(',') + 1);
        if ((*asmC)->LTbl.find(label) != (*asmC)->LTbl.end()) {
            lineaddr = (*asmC)->asmAddrTbl[linenum + 1]; //认为bne、beq的下一条还是指令
            labeladdr = (*asmC)->asmAddrTbl[(*asmC)->LTbl[label]];
            if(lineaddr >= labeladdr){
                off_imme_num = lineaddr - labeladdr;
                off_imme_num = 0 - off_imme_num;
            }
            else
                off_imme_num = labeladdr - lineaddr;
            off_imme_num = off_imme_num >> 1; // ofs/2->相隔的指令条数
            mcode = genIMcode(opc, rsnum, rtnum, 0, 0, off_imme_num, 3);
        }
        else
            ErrorMsg.assign("Error: Undefined Label used!\n");
        break;
            //rs,ofs
    case 1:
        rs = regs.substr(0, regs.find_first_of(','));
        rsnum = getRegisterNum(rs, rgs);
        label = regs.substr(regs.find_last_of(',') + 1);
        if ((*asmC)->LTbl.find(label) != (*asmC)->LTbl.end()) {
            lineaddr = (*asmC)->asmAddrTbl[linenum + 1]; //认为bne、beq的下一条还是指令
            labeladdr = (*asmC)->asmAddrTbl[(*asmC)->LTbl[label]];
            if(lineaddr >= labeladdr){
                off_imme_num = lineaddr - labeladdr;
                off_imme_num = 0 - off_imme_num;
            }
            else
                off_imme_num = labeladdr - lineaddr;
            off_imme_num = off_imme_num >> 1; // ofs/2->相隔的指令条数
            mcode = genIMcode(opc, rsnum, 17, 0, 0, off_imme_num, 3);
        }
        else
            ErrorMsg.assign("Error: Undefined Label used!\n");
        break;
    }
    return mcode;
}

static string genIMcode(int opc, int rsnum, int rtnum, int simme, unsigned int usimme, int offset, int choice)
{
    string str = "";
    bitset<6> opcb(opc);
    bitset<5> rsc(rsnum), rtc(rtnum);
    bitset<16> simmec(simme), usimmec(usimme), offsetc(offset);
    str += opcb.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rsc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    str += rtc.to_string<char, std::string::traits_type, std::string::allocator_type>();
    switch (choice)
    {
    case 1:
        str += simmec.to_string<char, std::string::traits_type, std::string::allocator_type>();
        break;
    case 2:
        str += usimmec.to_string<char, std::string::traits_type, std::string::allocator_type>();
        break;
    case 3:
        str += offsetc.to_string<char, std::string::traits_type, std::string::allocator_type>();
        break;
    }
    return str;
}

static string JtTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC)
{
    string tmpC, mcode;
    tmpC = code;
    mcode = "";
    string mne, label;
    int opc;
    unsigned int addr;
    if (tmpC[tmpC.find('j') + 1] == ' ') {
        mne = "j";
        label = tmpC.substr(tmpC.find_first_of('j') + 1);
    }
    else if (tmpC[tmpC.find('j') + 1] == 'a') {
        mne = "jal";
        label = tmpC.substr(tmpC.find_first_of('l') + 1);
    }
    else
        ErrorMsg.assign("Error: Undefined mnemonic used!\n");
    opc = ms->JtypeToOpcTbl[mne];
    label = trim(label);
    cout<< "jump to label: " << label <<endl;
    if ((*asmC)->LTbl.find(label) != (*asmC)->LTbl.end())
    {
        addr = ((*mcSet)->AddrTbl[(*asmC)->LTbl[label]]) >> 1;
        cout << "labelline: " << (*asmC)->LTbl[label] <<endl;
        cout << "labeladdr: " << addr <<endl;
        bitset<6> opcb(opc);
        bitset<26> addrc(addr);
        mcode += opcb.to_string<char, std::string::traits_type, std::string::allocator_type>();
        mcode += addrc.to_string<char, std::string::traits_type, std::string::allocator_type>();
        cout<<"j mccode:" << mcode <<endl;
    }
    else
        ErrorMsg.assign("Error: Undefined Label used!\n");
    return mcode;
}

//格式指令处理
/*
string insName[FINSNUM] = { "equ",".origin",".data",".text",".end",
    ".space",".zjie",".2zjie",".word" };
*/
static int isFormatCode(string s){
    //equ 定义常量表
    s.erase(s.find_last_not_of(' ') + 1, string::npos);    //去掉字符串末尾空格
    s.erase(0, s.find_first_not_of(' '));    //去掉字符串首空格
    if(s.find(" equ ") != string::npos || s.find('.') != string::npos)
        return 1;
    else
        return 0;
}

static void ParseFormatCode(string fcode, ACRec *asmC){
    //去掉字符串首尾空格
    fcode.erase(fcode.find_last_not_of(' ') + 1, string::npos);    //去掉字符串末尾空格
    fcode.erase(0, fcode.find_first_not_of(' '));    //去掉字符串首空格
    if(fcode.find('.') == string::npos)
    {
        //equ
        string vname = fcode.substr(0,fcode.find_first_of(' '));
        string vvalue = fcode.substr(fcode.find_last_of(' ')+1);
        (*asmC)->equTbl.insert(pair<string, string>(vname,vvalue));
    }
    else{
        string mnemonic;
        int id;
        mnemonic = fcode.substr(fcode.find('.'),fcode.find_last_of(' ') - fcode.find('.'));
        mnemonic = trim(mnemonic);
        id = ms->formatInstTbl[mnemonic];
        string vname,vvalue;
        unsigned int space;
        string variableArray;
        vector<string> vArray;
        string newOriginStr;
        unsigned int newOrigin;
        int vType,i,j,isH = 0;
        unsigned int vnum;
        string vstr;
        char vchar,hc,lc;
        switch (id) {
        case 1: //.origin
            newOriginStr = fcode.substr(fcode.find_last_of(' ')+1);
            if(newOriginStr.find("0x")!=string::npos)
                newOrigin = strtoul(newOriginStr.c_str(),NULL,16);
            else
                newOrigin = strtoul(newOriginStr.c_str(),NULL,10);
            curOrigin = newOrigin;
            PC = curOrigin;
            break;
        case 2://.data
            (*asmC)->dataStart = PC;
            break;
        case 3://.text
            (*asmC)->textStart = PC;
            break;
        case 4:
            (*asmC)->endAddr = PC;
        case 5://.space
            vname = fcode.substr(0,fcode.find_first_of(' '));
            vvalue = fcode.substr(fcode.find_last_of(' ')+1);
            if(vvalue.find("0x")!=string::npos)
                space = strtoul(newOriginStr.c_str(),NULL,16);
            else
                space = strtoul(newOriginStr.c_str(),NULL,10);
            PC += space;
            break;  
        case 6://.zjie
            variableArray = fcode.substr(fcode.find_last_of(' ')+1);
            vname = fcode.substr(0,fcode.find_first_of(' '));
            (*asmC)->variableTbl.insert(pair<string, unsigned int>(vname,PC));
            //这个变量表可能由字符串、数字、数字字母字符串成组成，不支持dup格式指令
            //字符串仅支持单引号表示，不支持双引号表示
            vArray = splitElement(variableArray);
            for(i = 0; i < vArray.size(); i++){
                vType = getVariableType(vArray[i]);
                cout << "vType: " << vType << ", v: " << vArray[i] << endl;
                switch(vType){
                case NUM:
                    //数字按无符号处理
                    if(vArray[i].substr(0,2) == "0x"){
                        vnum = strtoul(vArray[i].c_str(),NULL,16);
                    }
                    else
                        vnum = strtoul(vArray[i].c_str(),NULL,10);
                    MMap[PC++] = vnum & 0xFFFF;
                    break;
                case STR:
                    vstr = vArray[i].substr(1,vArray[i].find_last_of('\'')-1);
                    for(j = 0; j < vstr.length(); j++){
                        if((vstr[j]&0x80) == 0){
                            //ASCii
                            MMap[PC++] = vstr[j];
                        }
                        else if(isH == 0){
                            //HanZi
                            isH = 1;
                            hc = vstr[j];
                        }
                        else{
                            isH = 0;
                            lc = vstr[j];
                            MMap[PC++] = hc<<8 + lc;
                        }
                    }
                    break;
                case CHAR:
                    vchar = vArray[i][1];
                    MMap[PC++] = vchar;
                    break;
                default:
                    break;
                }
            }
            break;
        case 7://.2zjie
        case 8://.word
            variableArray = fcode.substr(fcode.find_last_of(' ')+1);
            vname = fcode.substr(0,fcode.find_first_of(' '));
            (*asmC)->variableTbl.insert(pair<string, unsigned int>(vname,PC));
            //这个变量表可能由字符串、数字、数字字母字符串成组成，不支持dup格式指令
            //字符串仅支持单引号表示，不支持双引号表示
            vArray = splitElement(variableArray);
            for(i = 0; i < vArray.size(); i++){
                vType = getVariableType(vArray[i]);
                cout << "vType: " << vType << ", v: " << vArray[i] << endl;
                switch(vType){
                case NUM:
                    //数字按无符号处理
                    if(vArray[i].substr(0,2) == "0x"){
                        vnum = strtoul(vArray[i].c_str(),NULL,16);
                    }
                    else
                        vnum = strtoul(vArray[i].c_str(),NULL,10);
                    cout<<vnum<<endl;
                    MMap[PC++] = vnum >> 16;
                    MMap[PC++] = vnum & 0xFFFF;
                    break;
                case STR:
                    vstr = vArray[i].substr(1,vArray[i].find_last_of('\'')-1);
                    for(j = 0; j < vstr.length(); j++){
                        if((vstr[j]&0x80) == 0){
                            //ASCii
                            MMap[PC++] = 0;
                            MMap[PC++] = vstr[j];
                        }
                        else if(isH == 0){
                            //HanZi
                            isH = 1;
                            hc = vstr[j];
                        }
                        else{
                            isH = 0;
                            lc = vstr[j];
                            MMap[PC++] = 0;
                            MMap[PC++] = hc<<8 + lc;
                        }
                    }
                    break;
                case CHAR:
                    vchar = vArray[i][1];
                    MMap[PC++] = 0;
                    MMap[PC++] = vchar;
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }
}

//记录机器码到内存单元
static void insertMCodeToMemory(unsigned int lineaddr,string machineCode){
    unsigned int mcnum = strtoul(machineCode.c_str(),NULL,2);
    unsigned short hi,lo;
    //大端存储
    hi = mcnum >> 16;
    lo = mcnum & 0xFFFF;
    MMap[lineaddr] = hi;
    MMap[lineaddr+1] = lo;
}

int containVariable(string exp, ACRec *asmC){
    //可能含有equ定义的变量或者是data端定义的变量
    //1.分离出其中的变量
    int contain = 0;
    vector<string> vc;
    string tmpv;
    regex pattern("[a-zA-Z_]\w{0,}");
    smatch res;
    string::const_iterator iterStart = exp.begin();
    string::const_iterator iterEnd = exp.end();
    while(regex_search(iterStart,iterEnd,res,pattern)){
        tmpv = res[0];
        vc.push_back(tmpv);
        iterStart = res[0].second;
    }
    //2.查表，equ类型直接字符串替换，其他类型把地址转成字符串进行替换
    for(int i = 0; i< vc.size();i++){
        if((*asmC)->equTbl.count(vc[i]) != 0){
            exp = exp.replace(exp.find(vc[i]),vc[i].length(),(*asmC)->equTbl[vc[i]]);
            contain = 1;
        }
        else if((*asmC)->variableTbl.count(vc[i]) != 0){
            unsigned int addr = (*asmC)->variableTbl[vc[i]];
            string addrs = udex2str(addr);
            exp = exp.replace(exp.find(vc[i]),vc[i].length(),addrs);
            contain = 1;
        }
    }
    return contain;
}

static int priority(int state, char a) {
    //计算操作符优先级的函数，注意state表示运算符状态：
    //state=1表示还未进栈，state=0表示栈内优先级，注意
    //这只对‘（’起作用
    int result;
    switch (a) {
    case '+':
    case '-':
        result = 1;
        break;
    case '*':
    case '/':
        result = 2;
        break;
    case '(':
        if (state == 0)
            result = 3;
        else
            result = 0;
        break;
    case '#':
        result = 0;
        break;
    default:
        break;
    }
    return result;
}

static double calculate(char op, double op1, double op2) {
    double result;
    switch (op) {
    case '+':
        result = op1 + op2;
        break;
    case '-':
        result = op1 - op2;
        break;
    case '*':
        result = op1*op2;
        break;
    case '/':
        result = op1 / op2;
        break;
    default:
        break;
    }
    return result;
}

//计算表达式的值
static int calculateExp(string exp, ACRec* asmC)
{
    string tmpE = exp;
    tmpE = trim(tmpE);
    if(containLetter(tmpE)){
        //是否包含变量需要处理格式指令之后写        
        if(containVariable(tmpE, asmC)){
            //用变量的值代替，写入表达式
            return calculateExp(tmpE,asmC);
        }
        //1.处理High、Low
        string ts;
        //将tmpE全部转化为大写字母
        transform(tmpE.begin(),tmpE.end(),back_inserter(ts),::toupper);
        if(ts.find("HIGH") != string::npos || ts.find("LOW") != string::npos){
            if(ts.find("HIGH") != string::npos){
                string nums = ts.substr(ts.find_last_of('H')+1);
                nums = trim(nums);
                return getHighBits(nums);
            }
            else
            {
                string nums = ts.substr(ts.find_last_of('W')+1);
                nums = trim(nums);
                return getLowBits(nums);
            }
        }
        else{
            ErrorMsg = "Error: undefined variable used!";
            return 0;
        }
    }
    //不含计算符号
    else if ((tmpE.find('+') == string::npos) && (tmpE.find('-') == string::npos)
        && (tmpE.find('/') == string::npos) && (tmpE.find('*') == string::npos)
        && (tmpE.find('(') == string::npos) && (tmpE.find(')') == string::npos))
    {
        if(tmpE.find('x')==string::npos)
            return atoi(tmpE.c_str());
        else
            return strtol(tmpE.c_str(),NULL,16);
    }
    //含计算符号
    else {
        //1.仅仅是负数
        if ((tmpE[0] == '-') && ((tmpE.find('+') == string::npos)
            && (tmpE.find('/') == string::npos) && (tmpE.find('*') == string::npos)
            && (tmpE.find('(') == string::npos) && (tmpE.find(')') == string::npos)))
        {
            tmpE.erase(0, 1);
            if(tmpE.find('x')==string::npos)
                return -atoi(tmpE.c_str());
            else
                return -strtol(tmpE.c_str(),NULL,16);
        }
        else {
            stack<char> operation;//存放操作符的栈
            stack<double> operand;//存放操作数的栈
            operation.push('#');//先将‘#’压栈，作为结尾符号
            string num;//存放操作数
            for (int i = 0; i < tmpE.length(); i++) {
                if (isdigit(tmpE[i])) {//出现数字
                    while (isdigit(tmpE[i]) || tmpE[i] == '.') {//将操作数提取完全
                        num.push_back(tmpE[i]);
                        i++;
                    }
                    double a = atof(num.c_str());//string->double
                    operand.push(a);//操作数入栈
                    num.clear();//num清空以备下次使用
                    i--;//位置还原
                }
                else if (tmpE[i] == '+' || tmpE[i] == '-' || tmpE[i] == '*' || tmpE[i] == '/' || tmpE[i] == '(') {//出现运算符
                    if (priority(0, tmpE[i])>priority(1, operation.top()))//优先级比较
                        operation.push(tmpE[i]);//>,直接入栈
                    else {
                        while (priority(0, tmpE[i]) <= priority(1, operation.top())) {//<,出栈并进行计算直至>
                            char temp = operation.top();
                            operation.pop();
                            double op2 = operand.top();
                            operand.pop();
                            double op1 = operand.top();
                            operand.pop();
                            operand.push(calculate(temp, op1, op2));
                        }
                        operation.push(tmpE[i]);//最后操作符入栈
                    }
                }
                else if (tmpE[i] == ')') {//扫描到‘）’
                    while (operation.top() != '(') {//出栈直至‘（’
                        char temp = operation.top();
                        operation.pop();
                        double op2 = operand.top();
                        operand.pop();
                        double op1 = operand.top();
                        operand.pop();
                        operand.push(calculate(temp, op1, op2));
                    }
                    operation.pop();//‘（’出栈
                }
                else {//非法字符的处理
                    ErrorMsg.assign("Error: IIlegal arithmatic expression.!\n");
                    return ERROR;
                }
            }
            while (operation.top() != '#') {//扫尾工作
                char temp = operation.top();
                operation.pop();
                double op2 = operand.top();
                operand.pop();
                double op1 = operand.top();
                operand.pop();
                operand.push(calculate(temp, op1, op2));
            }
            return (int)operand.top();//输出结果
        }
    }
}

string printMachineCode(MCSet* mcSet,int FileType)
{
    string printstr = "";
    char* tmp = new char[16];
    map<int, string>::iterator iter;
    iter = (*mcSet)->MCodeTbl.begin();
    while (iter != (*mcSet)->MCodeTbl.end()) {
        printf("%s\t#Addr:%x\n", StrToHex(iter->second).c_str(), (*mcSet)->AddrTbl[iter->first]);
        if(FileType == 0)
            printstr += iter->second + "\t#Addr:0x" + itoa((*mcSet)->AddrTbl[iter->first],tmp,16) + "\n";
        else
            printstr += StrToHex(iter->second) + "\t#Addr:0x" + itoa((*mcSet)->AddrTbl[iter->first],tmp,16) + "\n";
            iter++;
    }
    return printstr;
}

string printMachineCode()
{
    return ErrorMsg;
}

//ui汇编接口函数
ACRec* assembler(string content, string &result, int FileType)
{
    curOrigin = DEFAULTORIGIN;
    ErrorMsg = "No Error.\n";
    rgs = initRegTbls();
    ms = initMnemonicTbls();
    MCSet* mcSet = new MCSet();
    ACRec* asmC = new ACRec();
    (*asmC) = new struct AsmCodeRecord();
    (*mcSet) = new struct MachineCodeSet();
    string asmcode = content;
    firstScan(asmcode, asmC);
    genMachineCode(asmC, mcSet);
    if (ErrorMsg == "No Error.\n"){
        result = printMachineCode(mcSet ,FileType);
        return asmC;
    }
    else{
        result = printMachineCode();
        return NULL;
    }
}

void assembler(string program, unsigned short MemoryMap[], map<string, string> &macros,unsigned int &ds,unsigned int &cs, unsigned int &ce){
    string res;
    ACRec* asmC;
    memset(MMap,0,sizeof(unsigned short)*(0x3000));
    if((asmC = assembler(program,res,0))!=NULL){
        for(int i = 0; i < 0x3000; i++){
            MemoryMap[i] = MMap[i];
        }
        map<string, string>::iterator iter = (*asmC)->equTbl.begin();
        while(iter!=(*asmC)->equTbl.end()){
            macros.insert(pair<string,string>(iter->first,iter->second));
        }
        ds = (*asmC)->dataStart;
        cs = (*asmC)->textStart;
        ce = (*asmC)->endAddr;
    }
}

//ui汇编接口函数
bool assembler(string filename, string content, int choice, string &result, int FileType)
{
    ErrorMsg = "No Error.\n";
    rgs = initRegTbls();
    ms = initMnemonicTbls();
    PC = DEFAULTORIGIN;
    memset(MMap,0,sizeof(unsigned short)*(0x3000));
    MCSet* mcSet = new MCSet();
    ACRec* asmC = new ACRec();
    (*asmC) = new struct AsmCodeRecord();
    (*mcSet) = new struct MachineCodeSet();
    //string asmcode = "sw $s1,4*10($t0)";
    string asmcode;
    if(choice == 0)
        asmcode = readInputFile(filename);
    else
        asmcode = content;
    firstScan(asmcode, asmC);
    genMachineCode(asmC, mcSet);
    if (ErrorMsg == "No Error.\n"){
        result = printMachineCode(mcSet ,FileType);
        cout<<endl;
        for(int i = 0; i < 0x3000/16; i+=16){
            printf("%04x: %04x %04x %04x %04x %04x %04x %04x %04x - %04x %04x %04x %04x %04x %04x %04x %04x\n",\
                   i,MMap[i],MMap[i+1],MMap[i+2],MMap[i+3],MMap[i+4],MMap[i+5],MMap[i+6],MMap[i+7],MMap[i+8],MMap[i+9]\
                    ,MMap[i+10],MMap[i+11],MMap[i+12],MMap[i+13],MMap[i+14],MMap[i+15]);
        }
        return 1;
    }
    else{
        result = printMachineCode();
        return 0;
    }
}
