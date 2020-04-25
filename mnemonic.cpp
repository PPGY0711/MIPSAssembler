#include "mnemonic.h"

TotalMs initMnemonicTbls()
{
    TotalMs* Tms = new TotalMs();
    *Tms = new struct Mnemonics();

    createRtypeOpcTbl(Tms);
    createRtypeFuncTbl(Tms);
    createCtypeOpcTbl(Tms);
    createCtypeFuncTbl(Tms);
    createItypeOpcTbl(Tms);
    createJtypeOpcTbl(Tms);
    createPseudoTbl(Tms);
    createOpc2MneTbl(Tms);
    createR1Func2R1Tbl(Tms);
    createCRs2CMneTbl(Tms);
    createMne2CodeTbl(Tms);
    createFormatInstTbl(Tms);
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
static void createCtypeOpcTbl(TotalMs* Tms){
    int i;
    for (i = 0; i < CINSNUM; i++) {
        (*Tms)->CtypeToOpcTbl.insert(pair<string, int>((*Tms)->CtypeM[i], (*Tms)->CtypeOpc[i]));
    }
}

static void createCtypeFuncTbl(TotalMs* Tms){
    int i;
    for (i = 0; i < CINSNUM; i++) {
        (*Tms)->CtypeToFuncTbl.insert(pair<string, int>((*Tms)->CtypeM[i], (*Tms)->CtypeFunc[i]));
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
    for (i = 0; i < (RINSNUM -1); i++) {
        (*Tms)->r1Func2R1Tbl.insert(pair<int, string>((*Tms)->rmnesFuncs[i], (*Tms)->rmnes[i]));
    }
}

static void createCRs2CMneTbl(TotalMs* Tms){
    int i;
    for (i = 0; i < 3; i++) {
        (*Tms)->crs2mneTbl.insert(pair<int, string>((*Tms)->cmnesRs[i], (*Tms)->cmnes[i]));
    }
}

static void createMne2CodeTbl(TotalMs* Tms){
    int i;
    for (i = 0; i < RINSNUM+IINSNUM+JINSNUM+CINSNUM; i++) {
        (*Tms)->mne2codeTbl.insert(pair<string, int>((*Tms)->allmnemonics[i], i));
    }
}

static void createFormatInstTbl(TotalMs* Tms){
    int i;
    for(i = 0; i < FINSNUM; i++){
        (*Tms)->formatInstTbl.insert(pair<string, int>((*Tms)->insName[i],i));
    }
}

//enum InstType{R=0,I,J,C};
string getMnemonic(TotalMs Tms,int opc, int func, int rsnum, int* type){
    if(Tms->Opc2MnemonicTbl.count(opc) != 0 && Tms->Opc2MnemonicTbl[opc] != "R" && Tms->Opc2MnemonicTbl[opc] !="C"){
        if(Tms->Opc2MnemonicTbl[opc] == "j" || Tms->Opc2MnemonicTbl[opc] == "jal")
            (*type) = 2;//J
        else if(Tms->Opc2MnemonicTbl[opc] != "mul")
            (*type) = 1;//I
        return Tms->Opc2MnemonicTbl[opc];
    }
    else if(Tms->Opc2MnemonicTbl[opc] == "R"){
        (*type) = 0;//R
        if(Tms->Opc2MnemonicTbl[opc] == "mul")
            return Tms->Opc2MnemonicTbl[opc];
        else
            return Tms->r1Func2R1Tbl[func];
    }
    else if(Tms->Opc2MnemonicTbl[opc] == "C"){
        (*type) = 3;//C
        return Tms->crs2mneTbl[rsnum];
    }
    else
        return "NOTFOUND";
}
