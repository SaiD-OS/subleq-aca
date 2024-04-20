#include "monitor.h"

bool signalParser(uint8_t control, uint8_t mask) {
    return (control & mask) != 0;
}

void controlsignalsprint(uint8_t control, uint8_t printmask) {
    printf("Control Signals:\n");
    if(printmask & ZEROM)
        printf("zero signal: %s\t", signalParser(control, ZEROM) ? "true" : "false");
    if(printmask & MEMTOREGM)
        printf("memtoreg signal: %s\t", signalParser(control, MEMTOREGM) ? "true" : "false");
    if(printmask & ALUSRCM)
        printf("alusrc signal: %s\t", signalParser(control, ALUSRCM) ? "true" : "false");
    if(printmask & BRANCHM)
        printf("branch signal: %s\t", signalParser(control, BRANCHM) ? "true" : "false");
    if(printmask & MEMREADM)
        printf("memread signal: %s\t", signalParser(control, MEMREADM) ? "true" : "false");
    if(printmask & MEMWRITEM)
        printf("memwrite signal: %s\t", signalParser(control, MEMWRITEM) ? "true" : "false");
    if(printmask & REGWRITEM)
        printf("regwrite signal: %s\t", signalParser(control, REGWRITEM) ? "true" : "false");
    if(printmask & REGDESTM)
        printf("regdest signal: %s\t", signalParser(control, REGDESTM) ? "true" : "false");
    printf("\n");
}

void instructioninfo(uregint_t ir, regint_t *rf, char *stage) {
    #ifdef PROFILE    
        uint8_t opcode = ir >> 26;        
        uint8_t rs = (ir >> 21) & RSM;        
        uint8_t rt = (ir >> 16) & RTM;
        printf("Instruction Info::%s::\nopcode: %d, rs: %d, rt: %d, ", stage, opcode, rs, rt);
        if(opcode == R_OP) {
            uint8_t rd = (ir >> 11) & RDM;
            uint8_t funct = ir & FUNCTM;
            printf("rd: %d, funct: %d\n", rd, funct);
            printf("rs value: %d, rt value: %d, rd value: %d\n", rf[rs], rf[rt], rf[rd]);
        } else {
            immregint_t imm = ir & IMMEDM;
            printf("immediate value, :%d\n", imm);
            printf("rs value: %d, rt value: %d\n", rf[rs], rf[rt]);
        }
    #endif
}

void loggernorm(uregint_t ir, uregint_t pc, regint_t *rf) {
    #ifdef MONITOR
    printf("Program Counter: %d\n", pc);
    #endif
    instructioninfo(ir, rf, "NormalData");  
}

void loggerif(uregint_t ir, uregint_t pc, regint_t *rf, IFIDReg wrsv, EXMEMReg rdsv, int loglevel) {
    #ifdef MONITOR
    printf("Program Counter: %d\n", pc);
    if(loglevel == 4) {
        printf("Read Values in Instruction Fetch Stage:\nbranch address: %d\n", rdsv.branchAddress);
        controlsignalsprint(rdsv.control, 0b10010000);
    }
    if(loglevel >=3)    
        printf("Written values to IF - ID Stage Registers:\nprogram counter: %d, instruction: %x\n", wrsv.pc, wrsv.ir);
    #endif
    instructioninfo(ir, rf, "IFStage");  
}   

void loggerid(uregint_t ir, uregint_t pc, regint_t *rf, IDEXReg wrsv, IFIDReg rdsv, int loglevel) {
    #ifdef MONITOR
    printf("Program Counter: %d\n", pc);
    if(loglevel == 4) {
        printf("Read Values in Instruction Decode Stage:\nprogram counter: %u\tinstruction: %u\n", rdsv.pc, rdsv.ir);
        controlsignalsprint(wrsv.control, 0b01111111);
    }
    if(loglevel >= 3)
        printf("Written values to ID - EX Stage Registers:\nprogram counter: %u, instruction: %x, rt: %d, rd: %d, regRs: %u, regRt: %u, aluop: %d\n", wrsv.pc, wrsv.ir, wrsv.rt, wrsv.rd, wrsv.regRs, wrsv.regRt, wrsv.aluop);
    #endif
    instructioninfo(ir, rf, "IDStage");  
}

void loggerex(uregint_t ir, uregint_t pc, regint_t *rf, EXMEMReg wrsv, IDEXReg rdsv, int loglevel) {
    #ifdef MONITOR
    printf("Program Counter: %d\n", pc);
    if(loglevel == 4) {
        printf("Read Values in Execute Stage:\nregRs: %u, regRt: %u, signExt: %u, aluOp: %d\n", rdsv.regRs, rdsv.regRt, rdsv.signext, rdsv.aluop);
        controlsignalsprint(wrsv.control, 0b10100001);
    }
    if(loglevel >= 3) 
        printf("Written values to EX - MEM Stage Registers:\nALU Output: %u, Memory Output: %u, Branch Address: %u, destReg: %d\n", wrsv.aluOut, wrsv.memData, wrsv.branchAddress, wrsv.destReg);
    #endif
    instructioninfo(ir, rf, "EXStage");  
}

void loggermem(uregint_t ir, uregint_t pc, regint_t *rf, MEMWBReg wrsv, EXMEMReg rdsv, int loglevel) {
    #ifdef MONITOR
    printf("Program Counter: %d\n", pc);
    if(loglevel == 4) {
        printf("Read Values in Memory Stage:\nBranch Address: %u, ALU Output: %u, Memory Output: %u, destReg: %d\n", rdsv.branchAddress, rdsv.aluOut, rdsv.memData, rdsv.destReg);
        controlsignalsprint(rdsv.control, 0b00011100);
    }
    if(loglevel >= 3)
        printf("Written values to MEM - WB Stage Registers:\ndestReg: %d, ALU Output: %u, Memory Output: %u\n", wrsv.destReg, wrsv.aluOut, wrsv.memData);
    #endif
    instructioninfo(ir, rf, "MEMStage");  
}

void loggerwb(uregint_t ir, uregint_t pc, regint_t *rf, MEMWBReg rdsv, int loglevel) {
    #ifdef MONITOR
    printf("Program Counter: %d\n", pc);
    if(loglevel == 4) {
        printf("Read Values in Write Back Stage:\ndestReg: %d, ALU Output: %u, Memory Output: %u\n", rdsv.destReg, rdsv.aluOut, rdsv.memData);
        controlsignalsprint(rdsv.control, 0b01000000);
    }
    if(loglevel >= 3)
        printf("Final Result passed to write to the registers: %u\n", (rdsv.control & MEMTOREGM) == 0 ? rdsv.aluOut : rdsv.memData);
    #endif
    instructioninfo(ir, rf, "WBStage");  
}

void logger(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb, int loglevel) {
    if(stage == IFSTAGE)
        loggerif(ir, pc, rf, ifid[PSWR], exmem[PSRD], loglevel);
    else if(stage == IDSTAGE)
        loggerid(ir, pc, rf, idex[PSWR], ifid[PSRD], loglevel);
    else if(stage == EXESTAGE)
        loggerex(ir, pc, rf, exmem[PSWR], idex[PSRD], loglevel);
    else if(stage == MEMSTAGE)
        loggermem(ir, pc, rf, memwb[PSWR], exmem[PSRD], loglevel);
    else if(stage == WBSTAGE)
        loggerwb(ir, pc, rf, memwb[PSRD], loglevel);
    else
        loggernorm(ir, pc, rf);
}

void logerr(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 1)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb, 1);
}

void logperf(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 2)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb, 2);
}

void loginfo(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 3)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb, 3);
}

void logdebug(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 4)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb, 4);
}