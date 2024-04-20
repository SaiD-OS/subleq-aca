#include "arch.h"

#if ARCH != MIPS
    #include "subleqreg.h"
#endif

#if ARCH == SUBLEQB
    #include "subleqmem.h"
#endif

#if BENCHMARK == 1
    #include "../benchmarks/fibo.h"
#endif

//logging params
int stage = 0;
bool executeSubleq = false;

uregint_t pc, pcnext, subleqpc;
regint_t regFile[REG_NUMS];

IFIDReg *ifid = NULL; 
IDEXReg *idex = NULL;
EXMEMReg *exmem = NULL;
MEMWBReg *memwb = NULL;

regint_t result = 0;

memunit DM[MEM_SIZE];

void readRegfile(uint8_t reg1Loc, uint8_t reg2Loc, regint_t *reg1Val, regint_t *reg2Val) {
    *reg1Val = regFile[reg1Loc];
    *reg2Val = regFile[reg2Loc];
}

void writeRegfile(uint8_t regLoc, regint_t regVal, bool regWrite) {
    if (regWrite && regLoc) 
        regFile[regLoc] = regVal;	
}

uregint_t readIMM(uregint_t addr) {
    uregint_t instr = 0;
    for(int i=0; i<INCVAL; i++){
        instr += ((memunit)IM[addr + i]) << (REG_SIZE - (i+1)*8);
    }
    return instr;
}

int readDMM(int addr, bool memread) {
    int data = 0;
    if(memread) {
        for(int i=0; i<INCVAL; i++){
            data += DM[addr + i] << (REG_SIZE - (i+1)*8);
        }
    }
    return data;
}

void writeDMM(uregint_t addr, int data, bool memwrite) {
	if (memwrite) {
        for(int i=0; i<INCVAL; i++) {
            DM[addr+i] = (data >> (REG_SIZE - (i+1)*8)) & 0x000000FF;
        }
	}		
}

uregint_t incrementPC(uregint_t pc_var) {
    return pc_var + INCVAL;
}

regint_t addBranchAddr(regint_t pc_var, regint_t branchAddr) {
    return pc_var + branchAddr;
}

regint_t signExtn(immregint_t offset) {
    return (regint_t) offset;
}

void generateAluControl(uint8_t aluop, uint8_t funct, uint8_t *alucontrol) {
    if(aluop == 0) 
        *alucontrol = 0b00000010;
    else if(aluop == 1)
        *alucontrol = 0b00000110;
    else {
        funct = funct & 0xF;
        if(funct == 0)
            *alucontrol = 0b00000010;
        else if(funct == 2)
            *alucontrol = 0b00000110;
        else if(funct == 4)
            *alucontrol = 0b00000000;
        else if(funct == 5)
            *alucontrol = 0b00000001;
        else if(funct == 10)
            *alucontrol = 0b00000111;
        else
            *alucontrol = 0b00001111;
    }
}

regint_t alu(regint_t op1, regint_t op2, uint8_t alucontrol, uint8_t *control) {
    regint_t aluOut = 0;
    if(alucontrol==0)
        aluOut = op1 & op2;
    else if(alucontrol==1)
        aluOut = op1 | op2;
    else if(alucontrol==2)
        aluOut = op1 + op2;
    else if(alucontrol==6)
        aluOut = op1 - op2;
    else if(alucontrol==7) 
        if(op1 < op2)
            aluOut = 1;
        else
            aluOut = 0;
    else if(alucontrol==12)
        aluOut = ~(op1 | op2);
    
    if(aluOut == 0)
        setZeroSignal(control);

    return aluOut;    
}

void writeIFIDStageReg(uregint_t ir, uregint_t pc_var, bool regWrite) {
    if(regWrite) {
        ifid[PSWR].ir = ir;
        ifid[PSWR].pc = pc_var;
    }
}

void readIFIDStageReg(uregint_t *out_ir, uregint_t *out_pc) {
    *out_ir = ifid[PSRD].ir;
    *out_pc = ifid[PSRD].pc;
}

void writeIDEXStageReg(uregint_t ir, regint_t regRs, regint_t regRt, regint_t signext, uint8_t control, uint8_t aluop, uint8_t rt, uint8_t rd, uregint_t pc_var, bool regWrite) {
    if(regWrite) {
        idex[PSWR].ir = ir;
        idex[PSWR].regRs = regRs;
        idex[PSWR].regRt = regRt;
        idex[PSWR].signext = signext;
        idex[PSWR].control = control;
        idex[PSWR].aluop = aluop;
        idex[PSWR].rt = rt;
        idex[PSWR].rd = rd;
        idex[PSWR].pc = pc_var;
    }
}

void readIDEXStageReg(uregint_t *out_ir, regint_t *out_regRs, regint_t *out_regRt, regint_t *out_signext, uint8_t *out_control, uint8_t *out_aluop, uint8_t *out_rt, uint8_t *out_rd, uregint_t *out_pc) {
    *out_ir = idex[PSRD].ir;
    *out_regRs = idex[PSRD].regRs;
    *out_regRt = idex[PSRD].regRt;
    *out_signext = idex[PSRD].signext;
    *out_control = idex[PSRD].control;
    *out_aluop = idex[PSRD].aluop;
    *out_rt = idex[PSRD].rt;
    *out_rd = idex[PSRD].rd;
    *out_pc = idex[PSRD].pc;
}

void writeEXMEMStageReg(uregint_t ir, uint8_t control, uint8_t destReg, regint_t memData, regint_t aluOut, regint_t branchAddress, bool regWrite) {
    if(regWrite) {
        exmem[PSWR].ir = ir;
        exmem[PSWR].control = control;
        exmem[PSWR].destReg = destReg;
        exmem[PSWR].memData = memData;
        exmem[PSWR].aluOut = aluOut;
        exmem[PSWR].branchAddress = branchAddress;
    }
}

void readEXMEMStageReg(uregint_t *out_ir, uint8_t *out_control, uint8_t *out_destReg, regint_t *out_memData, regint_t *out_aluOut, regint_t *out_branchAddress) {
    *out_ir = exmem[PSRD].ir;
    *out_control = exmem[PSRD].control;
    *out_destReg = exmem[PSRD].destReg;
    *out_memData = exmem[PSRD].memData;
    *out_aluOut = exmem[PSRD].aluOut;
    *out_branchAddress = exmem[PSRD].branchAddress;
}

void writeMEMWBStageReg(uregint_t ir, uint8_t control, regint_t memData, regint_t aluOut, uint8_t destReg, bool regWrite) {
    if(regWrite) {
        memwb[PSWR].ir = ir;
        memwb[PSWR].control = control;
        memwb[PSWR].memData = memData;
        memwb[PSWR].aluOut = aluOut;
        memwb[PSWR].destReg = destReg;
    }
}

void readMEMWBStageReg(uregint_t *out_ir, uint8_t *out_control, regint_t *out_memData, regint_t *out_aluOut, uint8_t *out_destReg) {
    *out_ir = memwb[PSRD].ir;
    *out_control = memwb[PSRD].control;
    *out_memData = memwb[PSRD].memData;
    *out_aluOut = memwb[PSRD].aluOut;
    *out_destReg = memwb[PSRD].destReg;
}

void generateControlSignals(uint8_t opCode, uint8_t *control, uint8_t *aluop) {
    switch(opCode) {
        case ADDI_OP: 
            *control = 0b00100010;
            *aluop = 0;
            break;
        case LW_OP: 
            *control = 0b01101010;
            *aluop = 0;
            break;        
        case SW_OP: 
            *control = 0b00100100;
            *aluop = 0;
            break;
        case BNE_OP: 
            *control = 0b00010000;
            *aluop = 1;
            break;
        case R_OP: 
            *control = 0b00000011;
            *aluop = 2;
            break;
        default: 
            *control = NOOP;
            *aluop = 0;
    }
}

bool isSignal(uint8_t control, uint8_t mask) {
    return (control & mask) != 0;
}

void setZeroSignal(uint8_t *control) {
    *control = *control | 0b10000000;
}

uregint_t readsubleqinstr(uregint_t addr) {
    uregint_t instr = 0;
    for(int i=0; i<INCVAL; i++){
        instr += ((memunit)subleqmem[addr + i]) << (REG_SIZE - (i+1)*8);
    }
    return instr;
}

void readsubleqreg(uint8_t reg1Loc, uint8_t reg2Loc, regint_t *reg1Val, regint_t *reg2Val) {
    *reg1Val = subleqreg[reg1Loc];
    *reg2Val = subleqreg[reg2Loc];
}

void writesubleqreg(uint8_t regLoc, regint_t regVal, bool regWrite) {
    if (regWrite) 
        subleqreg[regLoc] = regVal;	
}

void loadsubleqsubroutine(uint8_t opc, uint8_t funct) {
    if(opc == R_OP) {
        executeSubleq = true;
        switch(funct) {
            case 32: subleqpc = 0; break;
        }
        printf("Perform Add operation ");
    } else if (opc == ADDI_OP) {
        executeSubleq = true;
        subleqpc = 0;
        printf("Perform Addi operation ");
    }
}

void setsubleqsource(regint_t src1, regint_t src2, bool regwrite) {
    if(regwrite) {
        subleqreg[SRC1] = src1;
        subleqreg[SRC2] = src2;
    }
}

regint_t getsubleqresult() {
    printf("Final subleq result: %d\n", subleqreg[DEST]);
    return subleqreg[DEST];
}

void executesubleqsubroutine() {
    size_t subleqinstrcnt = sizeof(subleqmem);
    printf("Executing subleq subroutine\n");
    while(subleqpc < subleqinstrcnt) {
        uregint_t subir = readsubleqinstr(subleqpc);
        uint8_t src1reg = (subir >> 21) & SRC1REG, src2reg = (subir >> 16) & SRC2REG;
        immregint_t branchval = subir & BRANCHVAL;
        regint_t src1val, src2val;
        readsubleqreg(src1reg, src2reg, &src1val, &src2val);
        regint_t res = src2val - src1val;
        printf("Subleq instr: PC: %d, SRC1: %d, SRC2: %d, Result: %d\n", subleqpc, src1val, src2val, res);
        writesubleqreg(src2reg, res, true);        
        if(res <= 0)
            subleqpc = branchval;
        else
            subleqpc = incrementPC(subleqpc); 
    }

    executeSubleq = false;
}

void instrfetch() {
    stage = 1;
    uregint_t ir = readIMM(pc);
    pcnext = incrementPC(pc);
    writeIFIDStageReg(ir, pcnext, true);
    
    logdebug(ir, pc, regFile, stage, ifid, idex, exmem, memwb);
}

void instrdecode() {
    
    stage = 2;
    uregint_t ifid_ir, ifid_pc;
    readIFIDStageReg(&ifid_ir, &ifid_pc);

    if(ifid_ir != NOOP) {
        uint8_t opc = ifid_ir >> 26;
        uint8_t rs = (ifid_ir >> 21) & RSM;
        uint8_t rt = (ifid_ir >> 16) & RTM;
        uint8_t rd = (ifid_ir >> 11) & RDM;
        uint8_t funct = ifid_ir & FUNCTM;
        immregint_t imm = ifid_ir & IMMEDM;
        uint8_t control, aluop;
        regint_t regRs, regRt, signext;
        
        loadsubleqsubroutine(opc, funct);
        
        generateControlSignals(opc, &control, &aluop);

        readRegfile(rs, rt, &regRs, &regRt);

        signext = signExtn(imm);       
        writeIDEXStageReg(ifid_ir, regRs, regRt, signext, control, aluop, rt, rd, ifid_pc, true);
        logdebug(ifid_ir, pc, regFile, stage, ifid, idex, exmem, memwb);
        
    } else {
        writeIDEXStageReg(ifid_ir, 0, 0, 0, 0, 0, 0, 0, ifid_pc, true);
    }
}

void execute() {
    stage = 3;
    uregint_t idex_ir, idex_pc;
    uint8_t idex_control, idex_aluop, idex_rt, idex_rd;
    regint_t idex_regRs, idex_regRt, idex_signext;
    readIDEXStageReg(&idex_ir, &idex_regRs, &idex_regRt, &idex_signext, &idex_control, &idex_aluop, &idex_rt, &idex_rd, &idex_pc);

    if(idex_ir != NOOP) {
        
        uregint_t branchaddress = (uregint_t) addBranchAddr(idex_pc, idex_signext << 2);
        uint8_t destReg = idex_rt, funct = (idex_signext & FUNCTM), aluctrl, control = idex_control;
        regint_t op2 = idex_regRt, aluout = 0;
        if(isSignal(idex_control, REGDESTM))
            destReg = idex_rd;
        if(isSignal(idex_control, ALUSRCM))
            op2 = idex_signext;

        if(executeSubleq) {
            printf(" with the arguements: %d and %d\n", idex_regRs, op2);
            setsubleqsource(idex_regRs, op2, true);
            executesubleqsubroutine();
            aluout = getsubleqresult();
        } else {
            generateAluControl(idex_aluop, funct, &aluctrl);

            aluout = alu(idex_regRs, op2, aluctrl, &control);
        }
        
        writeEXMEMStageReg(idex_ir, control, destReg, idex_regRt, aluout, branchaddress, true);
        logdebug(idex_ir, pc, regFile, stage, ifid, idex, exmem, memwb);
    } else {
        writeEXMEMStageReg(idex_ir, 0, 0, idex_regRt, 0, 0, true);
    }
}

void memoryaccess() {
    stage = 4;
    uregint_t exmem_ir;
    uint8_t exmem_control, exmem_destreg;
    regint_t exmem_memdata, exmem_aluout, exmem_branchaddr;
    readEXMEMStageReg(&exmem_ir, &exmem_control, &exmem_destreg, &exmem_memdata, &exmem_aluout, &exmem_branchaddr);
    if(exmem_ir != NOOP) {
        int memout;
        memout = readDMM(exmem_aluout, isSignal(exmem_control, MEMREADM));
        
        writeDMM(exmem_aluout, exmem_memdata, isSignal(exmem_control, MEMWRITEM));
        if(isSignal(exmem_control, BRANCHM) & !isSignal(exmem_control, ZEROM)) {
            pcnext = exmem_branchaddr;
        }
        writeMEMWBStageReg(exmem_ir, exmem_control, memout, exmem_aluout, exmem_destreg, true);
        logdebug(exmem_ir, pc, regFile, stage, ifid, idex, exmem, memwb);
    }  else {
        writeMEMWBStageReg(exmem_ir, exmem_control, 0, exmem_aluout, exmem_destreg, true);
    }
}

void writeback() {
    stage = 5;
    uregint_t memwb_ir;
    regint_t memwb_memdata, memwb_aluout;
    uint8_t memwb_control, memwb_destreg;
    readMEMWBStageReg(&memwb_ir, &memwb_control, &memwb_memdata, &memwb_aluout, &memwb_destreg);
    if(memwb_ir != NOOP) {
        regint_t result_var = memwb_aluout;
        if(isSignal(memwb_control, MEMTOREGM))
            result_var = memwb_memdata;
        writeRegfile(memwb_destreg, result_var, isSignal(memwb_control, REGWRITEM));
        logdebug(memwb_ir, pc, regFile, stage, ifid, idex, exmem, memwb);
    }
}

// Register Init with value 0, set pipeline registers, and pc to 0
void initregandmem() {
    
    memset(regFile, 0, REG_NUMS*sizeof(regint_t));
    memset(DM, 0, MEM_SIZE*sizeof(memunit));

    ifid = malloc(sizeof(IFIDReg)*2);
    ifid[PSWR].ir = NOOP;
    ifid[PSRD].ir = NOOP;
    idex = malloc(sizeof(IDEXReg)*2);
    idex[PSWR].ir = NOOP;
    idex[PSRD].ir = NOOP;
    exmem = malloc(sizeof(EXMEMReg)*2);
    exmem[PSWR].ir = NOOP;
    exmem[PSRD].ir = NOOP;
    memwb = malloc(sizeof(MEMWBReg)*2);
    memwb[PSWR].ir = NOOP;
    memwb[PSRD].ir = NOOP;

    pc = 0;
}

void preparenextcycle() {
    stage = 0;
    ifid[PSRD] = ifid[PSWR];
    idex[PSRD] = idex[PSWR];
    exmem[PSRD] = exmem[PSWR];
    memwb[PSRD] = memwb[PSWR];
    pc = pcnext;
}

void mipsexecution() {
    size_t instrcnt = sizeof(IM);
    
    initregandmem();
    while(pc < instrcnt) {
        //stages in every cycle        
        instrfetch();
        instrdecode();
        execute();
        memoryaccess();
        writeback();

        //update the stage registers
        preparenextcycle();
    }
}