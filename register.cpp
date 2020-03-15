#include "register.h"

Regs initRegTbls()
{
    Regs* Rs = new Regs();
    *Rs = new struct Registers();
    createRegCoWordTbl(Rs);
    createRegNumTbl(Rs);
    createRegWordTbl(Rs);
    return *Rs;
}

static void createRegCoWordTbl(Regs* Rs)
{
    int i;
    for (i = 0; i < COPRORNUM; i++) {
        (*Rs)->regCoWordTbl.insert(pair<string, int>((*Rs)->coprocessor[i], (*Rs)->regNumCo[i]));
    }
}
static void createRegWordTbl(Regs* Rs)
{
    int i;
    for (i = 0; i < CPURNUM; i++) {
        (*Rs)->regWordTbl.insert(pair<string, int>((*Rs)->regWordName[i], (*Rs)->regNum[i]));
    }
}
static void createRegNumTbl(Regs* Rs)
{
    int i;
    for (i = 0; i < CPURNUM; i++) {
        (*Rs)->regNumTbl.insert(pair<string, int>((*Rs)->regNumName[i], (*Rs)->regNum[i]));
    }
}

int getRegisterNum(string name, Regs Rs)
{
    if (Rs->regWordTbl.find(name) != Rs->regWordTbl.end())
        return Rs->regWordTbl[name];
    else if (Rs->regNumTbl.find(name) != Rs->regNumTbl.end())
        return Rs->regNumTbl[name];
    else
        return NOTEXIST;
}
