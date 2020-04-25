#include "disassembler.h"
#include "filehandler.h"
#include "stimulate.h"
#include <bitset>
#include <iostream>
#define ORIGIN 0x0000
#define ERROR -1
//控制用全局变量定义
//string ErrorMsg = "No Error.\n";
TotalMs dms;
Regs dRegs;
enum InstType{R=0,I,J,C};
map<string, unsigned int> LabelSet;
map<unsigned int, string> revLabelSet;

string parseStatement(string bincode, int lineNum){
    //cout << "here" << endl;
    string asmcode;
    unsigned int word;
    int iType = -1;
    int opc = binStr2dex(bincode.substr(0,6));
    int func = binStr2dex(bincode.substr(26,6));
    int r3num = binStr2dex(bincode.substr(6,5));
    string mnemonic = getMnemonic(dms, opc,func,r3num,&iType);
    cout << "mnemonic: " << mnemonic << " type: " << iType << endl;
    if(mnemonic == "ERROR"){
        //没有对应的就翻译成格式指令
        word = strtoul(bincode.c_str(),NULL,2);
        asmcode = ".word " + udex2str(word);
    }
    else{
        switch (iType) {
        case R:
            asmcode = RmcTranslate(bincode,mnemonic);
            break;
        case I:
            asmcode = ImcTranslate(bincode,mnemonic,lineNum);
            break;
        case J:
            asmcode = JmcTranslate(bincode,mnemonic,lineNum);
            break;
        case C:
            asmcode = CmcTranslate(bincode,mnemonic);
        }
    }
    return asmcode;
}

//指令翻译相关函数
static string RmcTranslate(string code, string mnemonic){
    int rsnum,rtnum,rdnum,sa;
    string rs,rt,rd,asmcode;

    rsnum = strtoul(code.substr(6,5).c_str(),NULL,2);
    rtnum = strtoul(code.substr(11,5).c_str(),NULL,2);
    rdnum = strtoul(code.substr(16,5).c_str(),NULL,2);
    sa = strtol(code.substr(21,5).c_str(),NULL,2);

    rs = dRegs->dRegWordTbl[rsnum];
    rt = dRegs->dRegWordTbl[rtnum];
    rd = dRegs->dRegWordTbl[rdnum];
    cout << "R: " << "rs: " << rs << "rt: " <<rt << "rd: " << rd << "sa: " << sa << endl;
    asmcode = "";
    int mnecode = dms->mne2codeTbl[mnemonic];
    /*
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
*/
    switch (mnecode) {
        //mne rd,rs,rt
    case 0: //case "add":
    case 1: //case "sub":
    case 2: //case "slt":
    case 3: //case "sltu":
    case 4: //case "and":
    case 5: //case "or":
    case 6: //case "xor":
    case 7: //case "nor":
    case 9: //case "sllv":
    case 11: //case "srlv":
    case 13: //case "srav":
    case 17: //case "mul":
        asmcode = mnemonic + " " + rd + ", " + rs + ", " + rt;
        break;
        //mne rd,rs,sa
    case 8: //case "sll":
    case 10: // case "srl":
    case 12: //case "sra":
        asmcode = mnemonic + " " + rd + "," + rs + ", ";
        char buf[20];
        itoa(sa,buf,10);
        asmcode += buf;
        break;
        //mne rs
    case 14: //case "jr":
    case 24: //case "mthi":
    case 25: //case "mtlo":
        asmcode = mnemonic + " " + rs;
        break;
    case 22: //case "mfhi":
    case 23: //case "mflo":
        //mne rd
        asmcode = mnemonic + " " + rd;
        break;
        //mne rs rd
    case 15: //case "jalr":
        asmcode = mnemonic + " " + rs + ", " + rd;
        break;
        //mne
    case 16: //case "syscall":
        asmcode = mnemonic;
        break;
        //mne rs,rt
    case 18: //case "mult":
    case 19: //case "multu":
    case 20: //case "div":
    case 21: //case "divu":
        asmcode = mnemonic + " " + rs + ", " + rt;
    default:
        break;
    }
    cout<< "asmcode: " << asmcode<<endl;
    return asmcode;
}

static string CmcTranslate(string code, string mnemonic){
    int rtnum,rcnum;
    string rt,rc,asmcode;
    rtnum = strtoul(code.substr(11,5).c_str(),NULL,2);
    rcnum = strtoul(code.substr(16,5).c_str(),NULL,2);
    rt = dRegs->dRegWordTbl[rtnum];
    rc = dRegs->dRegCoWordTbl[rcnum];
    cout << "C: " << "rt: " <<rt << "rc: " << rc << endl;
    asmcode = "";
    int mnecode = dms->mne2codeTbl[mnemonic];
    /*
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
*/
    switch (mnecode) {
        //mne rt,rc
    case 26: //case "mfc0":
    case 27: //case "mtc0":
        asmcode = mnemonic + " " + rt + ", " + rc;
        break;
        //mne
    case 28: //case "eret":
        asmcode = mnemonic;
        break;
    default:
        break;
    }
    cout<< "asmcode: " << asmcode<<endl;
    return asmcode;
}

static string ImcTranslate(string code, string mnemonic, int lineNum){
    int rsnum,rtnum,imme_ofs;
    unsigned int uimme_ofs;
    unsigned int absAddr;
    string Label = "";
    string rs,rt,s_imme_ofs, s_uimme_ofs, asmcode;
    rsnum = strtoul(code.substr(6,5).c_str(),NULL,2);
    rtnum = strtoul(code.substr(11,5).c_str(),NULL,2);
    imme_ofs = strtol(code.substr(16,16).c_str(),NULL,2);
    if(imme_ofs > 0x8000)
        imme_ofs -= 0x10000;
    uimme_ofs = strtoul(code.substr(16,16).c_str(),NULL,2);
    rs = dRegs->dRegWordTbl[rsnum];
    rt = dRegs->dRegWordTbl[rtnum];
    cout << "R: " << "rs: " << rs << "rt: " <<rt << "imme_ofs: " << imme_ofs << "uimme_ofs: "<< uimme_ofs << endl;
    asmcode = "";
    int mnecode = dms->mne2codeTbl[mnemonic];
    /*
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
*/
    switch (mnecode) {
        //mne rt,dat
    case 29: //case "lui":
        s_imme_ofs = dex2str(imme_ofs);
        asmcode = mnemonic + " " + rt + ", " + s_imme_ofs;
        break;
        //mne rt,rs,dat
    case 30: //case "addi":
    case 31: //case "slti":
    case 33: //case "andi":
    case 34: //case "ori":
    case 35: //case "xori":
        s_imme_ofs = dex2str(imme_ofs);
        asmcode = mnemonic + " " + rt + ", " + rs + ", "+ s_imme_ofs;
        break;
    case 32: //case "sltiu":
        s_uimme_ofs = udex2str(uimme_ofs);
        asmcode = mnemonic + " " + rt + ", " + rs + ", " + s_uimme_ofs;
        break;
        //mne rt,dat(rs)
    case 36: //case "lw":
    case 37: //case "lwx":
    case 38: //case "lh":
    case 39: //case "lhx":
    case 40: //case "lhu":
    case 41: //case "lhux":
    case 42: //case "sw":
    case 43: //case "swx":
    case 44: //case "sh":
    case 45: //case "shx":
        s_imme_ofs = dex2str(imme_ofs);
        asmcode = mnemonic + " " + rt + ", " + s_imme_ofs + "(" + rs +")";
        break;
        //mne rs,rt,ofs
    case 46: //case "beq":
    case 47: //case "bne":
        s_imme_ofs = dex2str(imme_ofs);
        absAddr = ORIGIN + ((lineNum+1) + imme_ofs)*2;
        if(revLabelSet.count(absAddr) == 0){
            Label = "L" + dex2str(LabelSet.size());
            LabelSet.insert(pair<string, unsigned int>(Label,absAddr));
            revLabelSet.insert(pair<unsigned int, string>(absAddr,Label));
            asmcode = mnemonic + " " + rs + ", " + rt + ", " + Label;
        }
        else
            asmcode = mnemonic + " " + rs + ", " + rt + ", " + revLabelSet[absAddr];
        break;
        //mne rs,ofs
    case 48: //case "bgezal":
        s_imme_ofs = dex2str(imme_ofs);
        absAddr = ORIGIN + ((lineNum+1) + imme_ofs)*2;
        if(revLabelSet.count(absAddr) == 0){
            Label = "L" + dex2str(LabelSet.size());
            LabelSet.insert(pair<string, unsigned int>(Label,absAddr));
            revLabelSet.insert(pair<unsigned int, string>(absAddr,Label));
            asmcode = mnemonic + " " + rs + ", " + Label;
        }
        asmcode = mnemonic + " " + rs + ", " + revLabelSet[absAddr];
        break;
    default:
        break;
    }
    return asmcode;
}

static string JmcTranslate(string code, string mnemonic ,int lineNum){
    unsigned int absAddr,PCplus4;
    PCplus4 = ORIGIN + lineNum * 2;
    bitset<32> bPCplus4(PCplus4);
    string asmcode = "";
    string sPCplus4 = bPCplus4.to_string();
    string sAbsAddr = sPCplus4.substr(0,4) + code.substr(6,26) + "0";
    absAddr = strtoul(sAbsAddr.c_str(),NULL,2);
    if(revLabelSet.count(absAddr) == 0){
        string Label = "L" + dex2str(LabelSet.size());
        LabelSet.insert(pair<string, unsigned int>(Label,absAddr));
        revLabelSet.insert(pair<unsigned int, string>(absAddr,Label));
        asmcode = mnemonic + " " + Label;
    }
    else
        asmcode = mnemonic + " " +revLabelSet[absAddr];
    return asmcode;
}

string disassembler(string machinecode){
    LabelSet.clear();
    revLabelSet.clear();
    dRegs = initRegTbls();
    dms = initMnemonicTbls();
    string mc = machinecode;
    getRidOfNotes(mc);
    vector<string> tmpmcv = splitC(mc,'\n');
    //取不为空的机器码
    map<unsigned int, string> mcv;
    string res = "";
    int lineNum = 0;
    for(int i = 0; i < tmpmcv.size(); i++){
        if(tmpmcv[i] != "")
        {
            string asmcode = parseStatement(tmpmcv[i],lineNum);
            mcv.insert(pair<unsigned int, string>(ORIGIN+lineNum*2,asmcode));
            lineNum++;
        }
    }
    cout<< "return main" <<endl;
    map<unsigned int, string>::iterator iter;
    iter = revLabelSet.begin();
    while (iter!=revLabelSet.end()) {
        cout<<"LabelSet - addr: " << iter->first <<endl;
        mcv[iter->first] = iter->second + ": " + mcv[iter->first];
        iter++;
    }
    map<unsigned int, string>::iterator iter2 = mcv.begin();
    while(iter2!=mcv.end()){
        cout<<"mcv - addr: " << iter2->first <<endl;
        res += iter2->second +'\n';
        iter2++;
    }
    cout<<"return to widget: " << res <<endl;
    return res;
}
