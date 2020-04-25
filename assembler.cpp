#pragma warning(disable:4996)
#include "assembler.h"
#include "mnemonic.h"
#include "register.h"
#include "filehandler.h"
#include <fstream>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <bitset>
#include <stack>
#include <math.h>
#include <string.h>
#include <iostream>
#define DAUFAULTORIGIN 0x0000
#define ERROR -99999
//控制用全局变量定义
string ErrorMsg = "No Error.\n";
TotalMs ms;
Regs rgs;

static int isPseudo(string s)
{
    string tmpm;
    if (s.find('$') != string::npos)
        if (s.find(':') == string::npos)
            tmpm = (s.substr(0, s.find_first_of('$')));
        else
            tmpm = (s.substr(s.find_first_of(':') + 1, s.find_first_of('$')-s.find_first_of(':')-1));
    else if (s.find("b ") != string::npos)
        tmpm = "b";
    tmpm = trim(tmpm);
    cout<< "isPseudo: " << tmpm <<endl;
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

    switch (which)
    {
    case 0://"push"
    case 1://"pop"
        nreg = c_in_str(s.c_str(), ',') + 1;
        regArray = splitC(regs, ',');

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
        regArray = splitC(regs, ',');
        newstr = "sra $at," + regArray[0] + ",31\n";
        newstr += "xor " + regs + ",$at\n";
        newstr += "sub " + regArray[0] + "," + regArray[0] + ",$at\n";
        break;
    case 13://"swap"
        regArray = splitC(regs, ',');
        newstr = "xor " + regArray[0] + "," + regArray[0] + "," + regArray[1] + "\n";
        newstr += "xor " + regArray[1] + "," + regArray[0] + "," + regArray[1] + "\n";
        newstr += "xor " + regArray[0] + "," + regArray[0] + "," + regArray[1] + "\n";
        break;
    case 14://"b"
        newstr = bstr;
        break;
    case 15://"beqz"
        regArray = splitC(regs, ',');
        newstr = "beq " + regArray[0] + ",$zero," + regArray[1] + "\n";
        break;
    case 16://"bnez"
        regArray = splitC(regs, ',');
        newstr = "bne " + regArray[0] + ",$zero," + regArray[1] + "\n";
        break;
    case 17://"beqi"
        regArray = splitC(regs, ',');
        newstr = "addi $at,$zero," + regArray[1] + "\n";
        newstr += "beq $at," + regArray[0] + "," + regArray[2] + "\n";
        break;
    case 18://"bnei"
        regArray = splitC(regs, ',');
        newstr = "addi $at,$zero," + regArray[1] + "\n";
        newstr += "bne $at," + regArray[0] + "," + regArray[2] + "\n";
        break;
    case 19://"blt"
        regArray = splitC(regs, ',');
        newstr = "slt $at," + regArray[0] + "," + regArray[1]+ "\n";
        newstr += "bne $at,$zero," + regArray[2] + "\n";
        break;
    case 20://"ble"
        regArray = splitC(regs, ',');
        newstr = "slt $at," + regArray[1] + "," + regArray[0] + "\n";
        newstr += "beq $at,$zero," + regArray[2] + "\n";
        break;
    case 21://"bgt"
        regArray = splitC(regs, ',');
        newstr = "slt $at," + regArray[1] + "," + regArray[0] + "\n";
        newstr += "bne $at,$zero," + regArray[2] + "\n";
        break;
    case 22://"bge"
        regArray = splitC(regs, ',');
        newstr = "slt $at," + regArray[0] + "," + regArray[1] + "\n";
        newstr += "beq $at,$zero," + regArray[2] + "\n";
        break;
    case 23://"seq"
        regArray = splitC(regs, ',');
        newstr = "sub " + regArray[0] + "," + regArray[1] + "," + regArray[2] + "\n";
        newstr += "sltiu " + regArray[0] + "," + regArray[1] + ",1\n";
        break;
    case 24://"sne"
        regArray = splitC(regs, ',');
        newstr = "sub " + regArray[0] + "," + regArray[1] + "," + regArray[2] + "\n";
        newstr += "sltu " + regArray[0] + ",$zero," + regArray[0] + "\n";
        break;
    }
    newstr = label + ": " + newstr;
    return newstr;
}

static int isEmpty(string s)
{
    if (trim(s) == "")
        return 1;
    return 0;
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
        else if(!(isEmpty(tmpcodePerLine[i])))
            codePerLine.push_back(tmpcodePerLine[i]);
    }
    //处理标号
    for (int i = 0; i < codePerLine.size(); i++) {
        cout<< "Line[" << i << "]:" << codePerLine[i] <<endl;
        if ((tmpid = codePerLine[i].find(':')) != string::npos)
        {
            if (!insertLabel(codePerLine[i], i, (*asmC)->LTbl)) {
                ErrorMsg.assign("Error: One Label Cannot Be Used Twice In One Segment!\n");
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
    //此版本未处理格式指令，令代码汇编初始地址都是0x0000
    //第二次扫描开始前，把有标号的行的地址先填到AddrTbl表中
    map<string, int>::iterator tmpit;
    tmpit = (*asmC)->LTbl.begin();
    while(tmpit!=(*asmC)->LTbl.end()){
        (*mcSet)->AddrTbl.insert(pair<int, unsigned int>(tmpit->second, DAUFAULTORIGIN + (tmpit->second * 2)));
        cout << tmpit->first << " " <<DAUFAULTORIGIN + (tmpit->second * 2) <<endl;
        tmpit++;
    }
    map<int, string>::iterator iter;
    iter = (*asmC)->CodeTbl.begin();
    while (iter != (*asmC)->CodeTbl.end()) {
        //1.生成每行指令的"绝对"地址（初始地址加上偏移）
        (*mcSet)->AddrTbl.insert(pair<int, unsigned int>(iter->first, DAUFAULTORIGIN + (iter->first * 2)));
        cout << "insert Addr at Line[" <<iter->first <<"] :" <<  DAUFAULTORIGIN + (iter->first * 2) <<endl;
        //2.翻译指令
        string tmpmenomic, machineCode;
        //不带寄存器符号的eret/syscall
        if (iter->second.find('$') == string::npos) {
            string tmps = iter->second;
            tmps = trim(tmps);
            if (tmps[0] == 'j') {
                machineCode = JtTranslate(iter->second, iter->first, mcSet, asmC);
                (*mcSet)->MCodeTbl.insert(pair<int, string>(iter->first, machineCode));
            }
            else {
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
            else
                ErrorMsg.assign("Error: Undefined mnemonic used!\n");
            (*mcSet)->MCodeTbl.insert(pair<int, string>(iter->first, machineCode));
        }
        iter++;
    }
}

//暂时仅考虑通用的32个寄存器，协处理器暂未考虑
static string RtTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC)
{
    string tmpC, mcode;
    tmpC = code;
    //"" eret/syscall
    if (tmpC.find('$') == string::npos) {
        if ((tmpC.find("eret") != string::npos) || tmpC.find("syscall") != string::npos) {
            tmpC = trim(tmpC);
            if (tmpC.compare("eret") == 0)
                return "01000010000000000000000000011000";
            else if (tmpC.compare("syscall") == 0)
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
                sa = calculateExp(saExp);
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
        case 16:	//rt,rc
            rt = regs.substr(0, regs.find_first_of(','));
            rtnum = getRegisterNum(rt, rgs);
            rd = regs.substr(regs.find_first_of(',') + 1);
            rdnum = getRegisterNum(rd, rgs);
            switch (mne[2]) {
            case 'f':
                if ((rtnum != NOTEXIST) && (rdnum != NOTEXIST))
                    mcode = genRMcode(opc, 0, rtnum, rdnum, 0, func);
                break;
            case 't':
                if ((rtnum != NOTEXIST) && (rdnum != NOTEXIST))
                    mcode = genRMcode(opc, 4, rtnum, rdnum, 0, func);
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

static string ItTranslate(string code, int linenum, MCSet* mcSet, ACRec* asmC)
{
    string tmpC, mcode;
    tmpC = code;
    string mne, rs, rt, rd, off_imme, label, regs;
    int opc, rsnum, rtnum, rdnum, off_imme_num;
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
        off_imme_num = calculateExp(off_imme);
        mcode = genIMcode(opc, 0, rtnum, off_imme_num, 0, 0, 1);
        break;
            //rt,rs,dat
    case 8:
    case 11:
    case 10:
    case 12:
    case 13:
    case 14:
        rt = regs.substr(0, regs.find_first_of(','));
        rtnum = getRegisterNum(rt, rgs);
        rs = regs.substr(regs.find_first_of(',') + 1, regs.find_last_of(',') - regs.find_first_of(',') - 1);
        rsnum = getRegisterNum(rs, rgs);
        off_imme = regs.substr(regs.find_last_of(',') + 1);
        off_imme_num = calculateExp(off_imme);
        if (opc != 11)
            mcode = genIMcode(opc, rsnum, rtnum, off_imme_num, 0, 0, 1);
        else
            mcode = genIMcode(opc, rsnum, rtnum, 0, off_imme_num, 0, 2);
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
        off_imme_num = calculateExp(off_imme);
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
            off_imme_num = (*asmC)->LTbl[label] - (linenum + 1);
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
            off_imme_num = (*asmC)->LTbl[label] - (linenum + 1);
            mcode = genIMcode(opc, rsnum, 17, 0, 0, off_imme_num, 3);
        }
        else
            ErrorMsg.assign("Error: Undefined Label used!\n");
        break;
    }
    return mcode;
}

static string genIMcode(int opc, int rsnum, int rtnum, int simme, int usimme, int offset, int choice)
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
        addr = ((*mcSet)->AddrTbl[(*asmC)->LTbl[label]]) / 2;
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
static int calculateExp(string exp)
{
    string tmpE = exp;
    tmpE = trim(tmpE);
    //不含计算符号
    if ((tmpE.find('+') == string::npos) && (tmpE.find('-') == string::npos)
        && (tmpE.find('/') == string::npos) && (tmpE.find('*') == string::npos)
        && (tmpE.find('(') == string::npos) && (tmpE.find(')') == string::npos))
    {
        if(tmpE.find('x')==string::npos)
            return atoi(tmpE.c_str());
        else
            return strtol(tmpE.c_str(),NULL,16);
    }
    //含计算符号（待完善）
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

//ui汇编接口函数
bool assembler(string filename, string content, int choice, string &result, int FileType)
{
    ErrorMsg = "No Error.\n";
    rgs = initRegTbls();
    ms = initMnemonicTbls();
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
        return 1;
    }
    else{
        result = printMachineCode();
        return 0;
    }
}
