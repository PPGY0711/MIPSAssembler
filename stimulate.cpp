#include "stimulate.h"
#include <QString>
#define LOOPTIMEMAX 2147483648
#define LOOPTIMEMIN 1
//中断处理函数
//1
void* print_int(Mipsc &mpc, void* a0, void* a1, void* a2){
    int* iptr = (int*)a0;
    printf("%d\n",(*iptr));
    mpc.MemoryMap[mpc.dPC] = (*iptr) >> 16;
    mpc.MemoryMap[mpc.dPC+1] = (*iptr) & 0x0000FFFF;
    mpc.dPC += 2;
    return NULL;
}
//11
void* print_char(Mipsc &mpc, void* a0, void* a1, void* a2){
    unsigned int *cptr = (unsigned int*)a0;
    unsigned short s = (*cptr) & 0xFFFF;
    char cs[3];
    if(s&0x8000){
        cs[0] = s >> 8;
        cs[1] = s&0xFF;
        cs[2] = '\0';
        printf("%s\n",cs);
    }
    else{
        cs[0] = s&0xFF;
        cs[1] = '\0';
        printf("%s\n",cs);
    }
    mpc.MemoryMap[mpc.dPC] = s;
    mpc.dPC += 1;
    return NULL;
}

Mipsc buildComputer(){
    MipscPtr Pmpc = new Mipsc();
    Pmpc->dPC = MEMORYSIZE - DISPLAYVOLMUE;
    Pmpc->PC = 0;
    Pmpc->rgs = initRegTbls();
    Pmpc->ms = initMnemonicTbls();
    Pmpc->rgs->Hi = Pmpc->rgs->Lo = 0;
    Pmpc->isDebugBegin = 0;
    Pmpc->lastPC = 0;
    Pmpc->intRequest = 0;
    Pmpc->intRetValuePtr = NULL;
    Pmpc->intTriggled = 0;
    memset(Pmpc->MemoryMap,0,sizeof(unsigned short)*(MEMORYSIZE+STACKSIZE));
    resetRegisterFile(Pmpc);
    setWelcomeStr((*Pmpc));
    return (*Pmpc);
}

void resetRegisterFile(MipscPtr Pmpc){
    memset(Pmpc->rgs->regContent,0,sizeof(unsigned int)*(CPURNUM));
    memset(Pmpc->rgs->regCoContent,0,sizeof(unsigned int)*(COPRORNUM));
    Pmpc->rgs->regContent[29] = Pmpc->rgs->regContent[30] = MEMORYSIZE+STACKSIZE + 1;//设置$sp,$fp
}

void resetPC(MipscPtr Pmpc){
    Pmpc->PC = Pmpc->cSegment;
    Pmpc->lastPC = Pmpc->PC;
}

void mcbuildAssembler(MipscPtr Pmpc, string program){
    //实现把传入的汇编程序翻译成成机器码存入内存数组的map中（包括数据和代码）
    assembler(program,Pmpc->MemoryMap,Pmpc->Macros,Pmpc->dSegment,Pmpc->cSegment,Pmpc->cEnd);
    Pmpc->PC = Pmpc->cSegment;//让PC指向代码段起点
    Pmpc->lastPC = Pmpc->PC;
}

void mcbuildDisAssembler(MipscPtr Pmpc, string program){
    //实现把传入的机器码存入内存数组的map中
    //简化情形，将程序从0x0000开始存放
    unsigned int addr = 0;
    vector<string> tmpMcPerLine = splitC(program,'\n');
    for(int i = 0; i < tmpMcPerLine.size(); i++){
        if(isEmpty(tmpMcPerLine[i]) != 0){
            unsigned int mc = strtoul(tmpMcPerLine[i].c_str(),NULL,2);
            Pmpc->MemoryMap[addr++] = mc>>16;
            Pmpc->MemoryMap[addr++] = mc & 0xffff;
        }
    }
}

static void emptyDisplayMemory(Mipsc &mpc){
    for(int i = MEMORYSIZE-DISPLAYVOLMUE; i < MEMORYSIZE;i++){
        mpc.MemoryMap[i] = 0;
    }
}

static void setWelcomeStr(Mipsc &mpc){
    emptyDisplayMemory(mpc);
    string tmpWelcome = " ----------MainWindow-------\n MIPS Simulator developed by PPGY0711!\n 在MIPS Assemble中键入MIPS汇编程序;\n 在MachineCode中键入机器码; \n 程序中打印的字符将会在此窗口显示!\n";
    QString tmp = QString::fromUtf8(tmpWelcome.c_str());
    string Welcome = tmp.toLocal8Bit().data();
//    for(int i = 0; i< Welcome.length();i++){
//        printf("char[%d]: %04x\n",i,Welcome[i]);
//    }
    int j = 0;
    int i = MEMORYSIZE-DISPLAYVOLMUE;
    int isH = 0;
    //ASCii和汉字都存成16个bit，汉字为GBK编码
    while(j<Welcome.length()){
        if((Welcome[j]&0x80) == 0){
            mpc.MemoryMap[i] = Welcome[j];
//            printf("Memory[%04x]:%04x\n",i,Welcome[j]);
            i++;
        }
        else{
            if(isH == 0){
                mpc.MemoryMap[i] = Welcome[j] & 0xFF;
                mpc.MemoryMap[i] <<= 8;
                isH = 1;
            }
            else{
                mpc.MemoryMap[i] |= Welcome[j] & 0xFF;
                isH = 0;
//                printf("Memory[%04x]:%04x\n",i,Welcome[j]);
                i++;
            }
        }
        j++;
    }
    mpc.MemoryMap[i] = '\0';//字符串结束标记
}

void ExecuteCode(Mipsc &mpc, string program, int strType){
    emptyDisplayMemory(mpc);
    //strType == 0 执行汇编代码
    if(strType == 0){
        mcbuildAssembler(&mpc,program);
    }
    //strType == 1 执行二进制机器码
    else{
        mcbuildDisAssembler(&mpc,program);
    }
    //代码存入内存后，开始执行
    _execCode(mpc, 0);
}

void DebugCode(Mipsc &mpc, string program,  int strType){
    if(mpc.isDebugBegin == 0){
        emptyDisplayMemory(mpc);
        //strType == 0 执行汇编代码
        if(strType == 0){
            mcbuildAssembler(&mpc,program);
        }
        //strType == 1 执行二进制机器码
        else{
            mcbuildDisAssembler(&mpc,program);
        }
        mpc.isDebugBegin = 1;
    }
    //代码存入内存后，开始执行
    else
        _execCode(mpc, 1);
}

static void _execCode(Mipsc &mpc, int execmode){
    //execmode 控制执行状态——0：执行；1：调试
    unsigned int mcode;
    unsigned int originPC;
    unsigned int looptime = 0;
    unsigned int maxlooptime = execmode == 0? LOOPTIMEMAX:LOOPTIMEMIN;
//    unsigned tmpPC = mpc.PC;
    while(mpc.PC <= mpc.cEnd && looptime < maxlooptime){
        originPC = mpc.PC;
        mpc.lastPC = mpc.PC;
        mpc.intTriggled = 0;
        mcode = mpc.MemoryMap[mpc.PC];
        mcode = mcode << 16;
        mcode = mcode + mpc.MemoryMap[mpc.PC+1];
        mpc.PC += 2;
        //取操作码、函数值
        unsigned int mnemonic = mcode >> 26;
        unsigned int func = mcode & 0x0000003F;
        unsigned int rsnum = (mcode & 0x03E00000) >> 21;
        unsigned int rtnum = (mcode & 0x001F0000) >> 16;
        unsigned int rdnum = (mcode & 0x0000F800) >> 11;
        unsigned int sanum = (mcode & 0x000007C0) >> 6;
        unsigned short uimme = mcode & 0x0000FFFF;
        //立即数
        short imme_ofs = mcode & 0x0000FFFF;
        unsigned int adr = mcode & 0x03FFFFFF;
        int extend_imme;
        unsigned extend_uimme;

        //操作数
        int rsc,rtc,rdc;
        rsc = mpc.rgs->regContent[rsnum];
        rtc = mpc.rgs->regContent[rtnum];
        rdc = mpc.rgs->regContent[rdnum];
        unsigned int ursc,urtc,urdc;
        ursc = mpc.rgs->regContent[rsnum];
        urtc = mpc.rgs->regContent[rtnum];
        urdc = mpc.rgs->regContent[rdnum];
        unsigned long long umulv,udivv;
        long long mulv,divv;
        //中断值
        unsigned int INT_num;
        unsigned int a0c,a1c,a2c,v0c;
        a0c = mpc.rgs->regContent[mpc.rgs->regWordTbl["$a0"]];
        a1c = mpc.rgs->regContent[mpc.rgs->regWordTbl["$a1"]];
        a2c = mpc.rgs->regContent[mpc.rgs->regWordTbl["$a2"]];
        v0c = mpc.rgs->regContent[mpc.rgs->regWordTbl["$v0"]];
        void* retvalue = NULL;
        switch (mnemonic) {
        case 0://R
            switch(func){
            case 32://add
                //有符号加，将寄存器内容解析成补码
                rdc = rsc+rtc;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 34://sub
                rdc = rsc-rtc;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 42://slt
                if(rsc<rtc)
                    mpc.rgs->regContent[rdnum] = 1;
                else
                    mpc.rgs->regContent[rdnum] = 0;
                break;
            case 43://sltu
                if(ursc<urtc)
                    mpc.rgs->regContent[rdnum] = 1;
                else
                    mpc.rgs->regContent[rdnum] = 0;
                break;
            case 36://and
                rdc = rsc & rtc;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 37://or
                rdc = rsc | rtc;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 38://xor
                rdc = rsc ^ rtc;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 39://nor
                rdc = ~(rsc | rtc);
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 0://sll
                //逻辑左移，0补位
                urdc = ursc << sanum;
                mpc.rgs->regContent[rdnum] = urdc;
                break;
            case 4://sllv
                //逻辑左移，0补位
                urdc = ursc << urtc;
                mpc.rgs->regContent[rdnum] = urdc;
                break;
            case 2://srl
                //逻辑右移，0补位
                urdc = ursc >> sanum;
                mpc.rgs->regContent[rdnum] = urdc;
                break;
            case 6://srlv
                //逻辑右移，0补位
                urdc = ursc >> urtc;
                mpc.rgs->regContent[rdnum] = urdc;
                break;
            case 3://sra
                //算术右移，以符号位补入
                rdc = rsc >> sanum;
                mpc.rgs->regContent[rdnum] = rdc;
            case 7://srav
                //逻辑右移，0补位
                rdc = rsc >> rtc;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 8://jr
                mpc.PC = ursc;
                break;
            case 9://jalr
                urdc = mpc.PC;
                mpc.PC = ursc;
                break;
            case 12://syscall
                //$v0 contains intRequest Num
                INT_num = v0c;
                retvalue = mpc.syscallFuncPtr[INT_num](mpc, (void*)&a0c,(void*)&a1c,(void*)&a2c);
                mpc.intRequest = INT_num;
                mpc.intRetValuePtr = retvalue;
                mpc.intTriggled = 1;
                break;
            case 24://mult
                mulv = (long long)rsc*rtc;
                mpc.rgs->Hi = (mulv >> 32) & 0xFFFFFFFF;
                mpc.rgs->Lo = mulv & 0xFFFFFFFF;
                break;
            case 25://multu
                umulv = (unsigned long long)ursc*urtc;
                mpc.rgs->Hi = (umulv >> 32) & 0xFFFFFFFF;
                mpc.rgs->Lo = umulv & 0xFFFFFFFF;
                break;
            case 26://div
                divv = ((long long)rsc) << 32;
                if(rtc != 0){
                    divv = divv / (long long)rtc;
                    mpc.rgs->Lo = (divv >> 32) & 0xFFFFFFFF;
                    mpc.rgs->Hi = divv & 0xFFFFFFFF;
                }
                else
                {
                    mpc.rgs->Hi = 0;
                    mpc.rgs->Lo = 0;
                }
                break;
            case 27://divu
                udivv = ((unsigned long long)ursc) << 32;
                if(urtc != 0){
                    udivv = udivv / (unsigned long long)urtc;
                    mpc.rgs->Lo = (udivv >> 32) & 0xFFFFFFFF;
                    mpc.rgs->Hi = udivv & 0xFFFFFFFF;
                }
                else
                {
                    mpc.rgs->Hi = 0;
                    mpc.rgs->Lo = 0;
                }
                break;
            case 16://mfhi
                rdc = mpc.rgs->Hi;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 17://mflo
                rdc = mpc.rgs->Lo;
                mpc.rgs->regContent[rdnum] = rdc;
                break;
            case 18://mthi
                mpc.rgs->Hi = rsc;
                break;
            case 19://mtlo
                mpc.rgs->Lo = rsc;
                break;
            default:
                break;
            }
            break;
        case 28://mul
            //短乘法
            rdc = rsc*rtc;
            mpc.rgs->regContent[rdnum] = rdc;
            break;
        case 16://C
            switch(rsnum){
            case 0://mfc0
                rdc = mpc.rgs->regCoContent[rdnum];
                rtc = rdc;
                break;
            case 4://mtc0
                rdc = rtc;
                mpc.rgs->regCoContent[rdnum] = rdc;
                break;
            case 16://eret
                mpc.PC = mpc.rgs->regCoContent[mpc.rgs->regCoWordTbl["EPC"]];
                break;
            default:
                break;
            }
            break;
            //I
        case 15: //lui
            extend_imme = imme_ofs;
            extend_imme = extend_imme << 16;
            mpc.rgs->regContent[rtnum] = extend_imme;
            break;
        case 8: //addi
            extend_imme = imme_ofs;
            rtc = rsc + extend_imme;
            mpc.rgs->regContent[rtnum] = rtc;
            break;
        case 10://slti
            extend_imme = imme_ofs;
            if(rsc < extend_imme)
                rtc = 1;
            else
                rtc = 0;
            mpc.rgs->regContent[rtnum] = rtc;
            break;
        case 11://sltiu
            extend_uimme = uimme;
            if(ursc < extend_uimme)
                urtc = 1;
            else
                urtc = 0;
            mpc.rgs->regContent[rtnum] = urtc;
            break;
        case 12://andi
            extend_uimme = uimme;
            urtc = ursc & extend_uimme;
            mpc.rgs->regContent[rtnum] = urtc;
            break;
        case 13://ori
            extend_uimme = uimme;
            urtc = ursc | extend_uimme;
            mpc.rgs->regContent[rtnum] = urtc;
            break;
        case 14://xori
            extend_uimme = uimme;
            urtc = ursc ^ extend_uimme;
            mpc.rgs->regContent[rtnum] = urtc;
            break;
        case 35://lw
            //big-end
            mpc.rgs->regContent[rtnum] = (mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs] << 16) + \
                    mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs+1];
            break;
        case 34://lwx
            //small-end
            mpc.rgs->regContent[rtnum] = (mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs+1] << 16) + \
                    mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs];
            break;
        case 33://lh
            //big-end
            mpc.rgs->regContent[rtnum] = mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs+1];
            if(mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs] & 0x8000){
                mpc.rgs->regContent[rtnum] += 0xFFFF0000; //最高位按符号位扩展
            }
            break;
        case 32://lhx
            //small-end
            mpc.rgs->regContent[rtnum] = mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs];
            if(mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs] & 0x8000){
                mpc.rgs->regContent[rtnum] += 0xFFFF0000; //最高位按符号位扩展
            }
            break;
        case 37://lhu
            //big-end
            mpc.rgs->regContent[rtnum] = mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs+1];
            break;
        case 36://lhux
            //small-end
            mpc.rgs->regContent[rtnum] = mpc.MemoryMap[mpc.rgs->regContent[rsnum]+imme_ofs];
            break;
        case 43://sw
            //big-end
            mpc.MemoryMap[mpc.rgs->regContent[rsnum] + imme_ofs] = mpc.rgs->regContent[rtnum]>>16;
            mpc.MemoryMap[mpc.rgs->regContent[rsnum] + imme_ofs + 1] = mpc.rgs->regContent[rtnum]&0xFFFF;
            break;
        case 42://swx
            //small-end
            mpc.MemoryMap[mpc.rgs->regContent[rsnum] + imme_ofs + 1] = mpc.rgs->regContent[rtnum]>>16;
            mpc.MemoryMap[mpc.rgs->regContent[rsnum] + imme_ofs] = mpc.rgs->regContent[rtnum]&0xFFFF;
            break;
        case 41://sh
            //big-end
            mpc.MemoryMap[mpc.rgs->regContent[rsnum] + imme_ofs + 1] = mpc.rgs->regContent[rtnum]&0xFFFF;
            break;
        case 40://shx
            //small-end
            mpc.MemoryMap[mpc.rgs->regContent[rsnum] + imme_ofs] = ((mpc.rgs->regContent[rtnum]&0x00FF) << 8)\
                    + ((mpc.rgs->regContent[rtnum]&0xFF00) >> 8); //高低八位交换
            break;
        case 4://beq
            if(rsc == rtc){
                extend_imme = imme_ofs;
                mpc.PC = mpc.PC + (extend_imme << 1);
            }
            break;
        case 5://bne
            if(rsc != rtc){
                extend_imme = imme_ofs;
                mpc.PC = mpc.PC + (extend_imme << 1);
            }
            break;
        case 1://bgezal
            extend_imme = imme_ofs;
            if(rsc >= 0){
                mpc.rgs->regContent[mpc.rgs->regWordTbl["$ra"]] = mpc.PC;
                mpc.PC = mpc.PC + (extend_imme << 1);
            }
            break;
            //J
        case 2://j
            mpc.PC = (mpc.PC & 0xF8000000) | (adr << 1);
            break;
        case 3://jal
            mpc.rgs->regContent[mpc.rgs->regWordTbl["$ra"]] = mpc.PC;
            mpc.PC = (mpc.PC & 0xF8000000) | (adr << 1);
            break;
        default:
            break;
        }
        looptime++;
        if(mpc.PC == originPC)
            break; //避免死循环
    }
    if(mpc.PC > mpc.cEnd)
        mpc.isDebugBegin = 0;
}
