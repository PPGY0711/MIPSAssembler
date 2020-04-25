#include "mnemonic.h"

TotalMs initMnemonicTbls()
{
    TotalMs* Tms = new TotalMs();
    *Tms = new struct Mnemonics();
    createRtypeOpcTbl(Tms);
    createRtypeFuncTbl(Tms);
    createItypeOpcTbl(Tms);
    createJtypeOpcTbl(Tms);
    createPseudoTbl(Tms);
    createOpc2MneTbl(Tms);
    createR1Func2R1Tbl(Tms);
    createR2Func2R2Tbl(Tms);
    createR3Rs2R3Tbl(Tms);
    createMne2CodeTbl(Tms);
    return *Tms;
}

static void createRtypeOpcTbl(TotalMs* Tms)
{
    int i;
    for (i = 0; i < RINSNUM; i++) {
        (*Tms)->RtypeToOpcTbl.insert(pair<string, int>((*Tms)->RtypeM[i], (*Tms)->RtypeOpc[i]));
    }
}
static void createRtypeFuncTbl(TotalMs* Tms)
{
    int i;
    for (i = 0; i < RINSNUM; i++) {
        (*Tms)->RtypeToFuncTbl.insert(pair<string, int>((*Tms)->RtypeM[i], (*Tms)->RtypeFunc[i]));
    }
}
static void createItypeOpcTbl(TotalMs* Tms)
{
    int i;
    for (i = 0; i < IINSNUM; i++) {
        (*Tms)->ItypeToOpcTbl.insert(pair<string, int>((*Tms)->ItypeM[i], (*Tms)->ItypeOpc[i]));
    }
}
static void createJtypeOpcTbl(TotalMs* Tms)
{
    int i;
    for (i = 0; i < JINSNUM; i++) {
        (*Tms)->JtypeToOpcTbl.insert(pair<string, int>((*Tms)->JtypeM[i], (*Tms)->JtypeOpc[i]));
    }
}

static void createPseudoTbl(TotalMs* Tms) {
    int i;
    for (i = 0; i < PSEUDO; i++) {
        (*Tms)->PseudoTbl.insert(pair<string, int>((*Tms)->pseudoM[i], i));
    }
}

static void createOpc2MneTbl(TotalMs* Tms){
    int i;
    for (i = 0; i < IINSNUM+JINSNUM+3; i++) {
        (*Tms)->Opc2MnemonicTbl.insert(pair<int, string>((*Tms)->opcs[i], (*Tms)->mnemonics[i]));
    }
}

static void createR1Func2R1Tbl(TotalMs* Tms){
    int i;
    for (i = 0; i < (RINSNUM -4); i++) {
        (*Tms)->r1Func2R1Tbl.insert(pair<int, string>((*Tms)->r1sFuncs[i], (*Tms)->r1s[i]));
    }
}

static void createR2Func2R2Tbl(TotalMs* Tms){
    int i;
    for (i = 0; i < 1; i++) {
        (*Tms)->r2Func2R2Tbl.insert(pair<int, string>((*Tms)->r2sFuncs[i], (*Tms)->r2s[i]));
    }
}

static void createR3Rs2R3Tbl(TotalMs* Tms){
    int i;
    for (i = 0; i < 3; i++) {
        (*Tms)->r3Rs2R3Tbl.insert(pair<int, string>((*Tms)->r3sRs[i], (*Tms)->r3s[i]));
    }
}

static void createMne2CodeTbl(TotalMs* Tms){
    int i;
    for (i = 0; i < RINSNUM+IINSNUM+JINSNUM; i++) {
        (*Tms)->mne2codeTbl.insert(pair<string, int>((*Tms)->allmnemonics[i], i));
    }
}

//enum InstType{R=0,I,J};
string getMnemonic(TotalMs Tms,int opc, int func, int r3num, int* type){
    if(Tms->Opc2MnemonicTbl.count(opc) != 0 && Tms->Opc2MnemonicTbl[opc] != "R1" && Tms->Opc2MnemonicTbl[opc] !="R2" && Tms->Opc2MnemonicTbl[opc] != "R3"){
        if(Tms->Opc2MnemonicTbl[opc] == "j" || Tms->Opc2MnemonicTbl[opc] == "jal")
            (*type) = 2;//J
        else
            (*type) = 1;//I
        return Tms->Opc2MnemonicTbl[opc];
    }
    else if(Tms->Opc2MnemonicTbl[opc] == "R1" || Tms->Opc2MnemonicTbl[opc] =="R2" || Tms->Opc2MnemonicTbl[opc] == "R3"){
        (*type) = 0;//R
        if(Tms->Opc2MnemonicTbl[opc] == "R1")
            return Tms->r1Func2R1Tbl[func];
        else if(Tms->Opc2MnemonicTbl[opc] == "R2")
            return Tms->r2Func2R2Tbl[func];
        else
            return Tms->r3Rs2R3Tbl[r3num];
    }
    else{
        (*type) = -1;
        return "ERROR";
    }
}
