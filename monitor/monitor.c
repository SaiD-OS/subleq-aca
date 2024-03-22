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
    #ifdef MONITOR    
        uint8_t opcode = ir >> 26;        
        uint8_t rs = (ir >> 21) & RSM;        
        uint8_t rt = (ir >> 16) & RTM;
        printf("Instruction Info::%s::\n, opcode: %d, rs: %d, rt: %d, ", stage, opcode, rs, rt);
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
    printf("Program Counter: %d\n", pc);
    instructioninfo(ir, rf, "NormalData");  
}

void loggerif(uregint_t ir, uregint_t pc, regint_t *rf, IFIDReg wrsv, EXMEMReg rdsv) {
    printf("Program Counter: %d\n", pc);
    printf("Read Values in Instruction Fetch Stage:\nbranch address: %d\n", rdsv.branchAddress);
    controlsignalsprint(rdsv.control, 0b10010000);
    printf("Written values to IF - ID Stage Registers:\nprogram counter: %d, instruction: %x\n", wrsv.pc, wrsv.ir);
    instructioninfo(ir, rf, "IFStage");  
}   

void loggerid(uregint_t ir, uregint_t pc, regint_t *rf, IDEXReg wrsv, IFIDReg rdsv) {
    printf("Program Counter: %d\n", pc);
    printf("Read Values in Instruction Decode Stage:\nprogram counter: %s\tinstruction: %s\n", rdsv.pc, rdsv.ir);
    controlsignalsprint(wrsv.control, 0b01111111);
    printf("Written values to ID - EX Stage Registers:\nprogram counter: %d, instruction: %x, rt: %d, rd: %d, regRs: %d, regRt: %d, aluop: %d\n", wrsv.pc, wrsv.ir, wrsv.rt, wrsv.rd, wrsv.regRs, wrsv.regRt, wrsv.aluop);
    instructioninfo(ir, rf, "IDStage");  
}

void loggerex(uregint_t ir, uregint_t pc, regint_t *rf, EXMEMReg wrsv, IDEXReg rdsv) {
    printf("Program Counter: %d\n", pc);
    // printf("Read Values in Execute Stage:\nbranch signal: %s\tzero signal: %s\tbranch address: %d\n", );
    // printf("Written values to EX - MEM Stage Registers:\nprogram counter: %d, instruction: %x\n", wrsv.pc, wrsv.ir);
    instructioninfo(ir, rf, "EXStage");  
}

void loggermem(uregint_t ir, uregint_t pc, regint_t *rf, MEMWBReg wrsv, EXMEMReg rdsv) {
    printf("Program Counter: %d\n", pc);
    // printf("Read Values in Instruction Fetch Stage:\nbranch signal: %s\tzero signal: %s\tbranch address: %d\n", strbool(isSignal(rdsv.control, BRANCHM)), strbool(isSignal(rdsv.control, ZEROM)), rdsv.branchAddress);
    // printf("Written values to IF - ID Stage Registers:\nprogram counter: %d, instruction: %x\n", wrsv.pc, wrsv.ir);
    instructioninfo(ir, rf, "MEMStage");  
}

void loggerwb(uregint_t ir, uregint_t pc, regint_t *rf, IFIDReg wrsv, MEMWBReg rdsv) {
    printf("Program Counter: %d\n", pc);
    // printf("Read Values in Instruction Fetch Stage:\nbranch signal: %s\tzero signal: %s\tbranch address: %d\n", strbool(isSignal(rdsv.control, BRANCHM)), strbool(isSignal(rdsv.control, ZEROM)), rdsv.branchAddress);
    // printf("Written values to IF - ID Stage Registers:\nprogram counter: %d, instruction: %x\n", wrsv.pc, wrsv.ir);
    instructioninfo(ir, rf, "WBStage");  
}

void logger(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(stage == IFSTAGE)
        loggerif(ir, pc, rf, ifid[PSWR], exmem[PSRD]);
    else if(stage == IDSTAGE)
        loggerid(ir, pc, rf, idex[PSWR], ifid[PSRD]);
    else if(stage == EXESTAGE)
        loggerex(ir, pc, rf, exmem[PSWR], idex[PSRD]);
    else if(stage == MEMSTAGE)
        loggermem(ir, pc, rf, memwb[PSWR], exmem[PSRD]);
    else if(stage == WBSTAGE)
        loggerwb(ir, pc, rf, ifid[PSWR], memwb[PSRD]);
    else
        loggernorm(ir, pc, rf);
}

void logerr(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 1)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb);
}

void logperf(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 2)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb);
}

void loginfo(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 3)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb);
}

void logdebug(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb) {
    if(LOGLEVEL >= 4)
        logger(ir, pc, rf, stage, ifid, idex, exmem, memwb);
}