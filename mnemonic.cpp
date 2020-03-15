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

