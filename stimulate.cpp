#include "stimulate.h"
#include "assembler.h"
#include "disassembler.h"
#include "filehandler.h"
Mipsc buildComputer(){
    MipscPtr Pmpc = new Mipsc();
    Pmpc->dPC = 0x3000;
    Pmpc->PC = 0;
    Pmpc->rgs = initRegTbls();
    memset(Pmpc->MemoryMap,0,sizeof(unsigned short)*(0x4000));
    return (*Pmpc);
}

//中断处理函数
void* print_int(void* a0, void* a1, void* a2);      //1
void* print_string(void* a0, void* a1, void* a2);   //4
void* read_int(void* a0, void* a1, void* a2);       //5
void* read_string(void* a0, void* a1, void* a2);    //8
void* sbrk(void* a0, void* a1, void* a2);           //9
void* exit(void* a0, void* a1, void* a2);           //10
void* print_char(void* a0, void* a1, void* a2);     //11
void* read_char(void* a0, void* a1, void* a2);      //12
void* open(void* a0, void* a1, void* a2);           //13
void* read(void* a0, void* a1, void* a2);           //14
void* write(void* a0, void* a1, void* a2);          //15
void* close(void* a0, void* a1, void* a2);          //16
void* exit2(void* a0, void* a1, void* a2);           //17

void mcbuildAssembler(MipscPtr Pmpc, string program){
    //实现把传入的汇编程序翻译成成机器码存入内存数组的map中（包括数据和代码）
    assembler(program,Pmpc->MemoryMap,Pmpc->Macros,Pmpc->dSegment,Pmpc->cSegment);
    Pmpc->PC = Pmpc->cSegment;//让PC指向代码段起点
}

void mcbuildDisAssembler(Mipsc Pmpc, string program){
    //实现把传入的机器码存入内存数组的map中
    //简化情形，将程序从0x0000开始存放
    unsigned int addr = 0;
    vector<string> tmpMcPerLine = splitC(program,'\n');
    for(int i = 0; i < tmpMcPerLine.size(); i++){
        if(isEmpty(tmpMcPerLine[i]) != 0){
            unsigned int mc = strtoul(tmpMcPerLine[i].c_str(),NULL,2);
            Pmpc.MemoryMap[addr++] = mc>>16;
            Pmpc.MemoryMap[addr++] = mc & 0xffff;
        }
    }
}
