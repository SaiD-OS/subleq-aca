#ifndef ARCH_H
#define ARCH_H

#include "../config/config.h"
#include "../monitor/monitor.h"
#include <stdbool.h>

//Instruction
#define NOOP 0x00000000

//Opcodes MIPS
#define R_OP 0
#define ADDI_OP 8
#define LW_OP 35
#define SW_OP 43
#define BNE_OP 5

//Defining Control Signals Masks for simplicity
//MEMTOREG || ALUSRC || BRANCH || MEMREAD || MEMWRITE || REGWRITE || REGDEST
#define REGDESTM 0b00000001
#define REGWRITEM 0b00000010
#define MEMWRITEM 0b00000100
#define MEMREADM 0b00001000
#define BRANCHM 0b00010000
#define ALUSRCM 0b00100000
#define MEMTOREGM 0b01000000
#define ZEROM 0b10000000

//Intruction Decode Masks
#define SHAMTM 0x0000001F
#define RTM 0x0000001F
#define RDM 0x0000001F
#define RSM 0x0000001F
#define FUNCTM 0x0000003F
#define IMMEDM 0x0000FFFF

//Subleq instruction masks
#define SRC1REG 0x0000001F
#define SRC2REG 0x0000001F
#define BRANCHVAL 0x0000FFFF

void readRegfile(uint8_t reg1Loc,  uint8_t reg2Loc, regint_t *reg1Val, regint_t *reg2Val);

void writeRegfile(uint8_t regLoc, regint_t regVal, bool regWrite);

uregint_t readIMM(uregint_t addr);

int readDMM(int addr, bool memread);

void writeDMM(uregint_t addr, int data, bool memwrite);

uregint_t incrementPC(uregint_t pc);

regint_t addBranchAddr(regint_t pc, regint_t branchAddr);

regint_t signExtn(immregint_t offset);

void generateAluControl(uint8_t aluop, uint8_t funct, uint8_t *alucontrol);

regint_t alu(regint_t op1, regint_t op2, uint8_t alucontrol, uint8_t *control);

void writeIFIDStageReg(uregint_t ir, uregint_t pc, bool regWrite);

void readIFIDStageReg(uregint_t *out_ir, uregint_t *out_pc);

void writeIDEXStageReg(uregint_t ir, regint_t regRs, regint_t regRt, regint_t signext, uint8_t control, uint8_t aluop, uint8_t rt, uint8_t rd, uregint_t pc, bool regWrite);

void readIDEXStageReg(uregint_t *out_ir, regint_t *out_regRs, regint_t *out_regRt, regint_t *out_signext, uint8_t *out_control, uint8_t *out_aluop, uint8_t *out_rt, uint8_t *out_rd, uregint_t *out_pc);

void writeEXMEMStageReg(uregint_t ir, uint8_t control, uint8_t destReg, regint_t memData, regint_t aluOut, regint_t branchAddress, bool regWrite);

void readEXMEMStageReg(uregint_t *out_ir, uint8_t *out_control, uint8_t *out_destReg, regint_t *out_memData, regint_t *out_aluOut, regint_t *out_branchAddress);

void writeMEMWBStageReg(uregint_t ir, uint8_t control, regint_t memData, regint_t aluOut, uint8_t destReg, bool regWrite);

void readMEMWBStageReg(uregint_t *out_ir, uint8_t *out_control, regint_t *out_memData, regint_t *out_aluOut, uint8_t *out_destReg);

void generateControlSignals(uint8_t opCode, uint8_t *control, uint8_t *aluop);

bool isSignal(uint8_t control, uint8_t mask);

void setZeroSignal(uint8_t *control);

void instrfetch();

void instrdecode();

void execute();

void memoryaccess();

void writeback();

void initregandmem();

void preparenextcycle();

uregint_t readsubleqinstr(uregint_t addr);

void readsubleqreg(uint8_t reg1Loc, uint8_t reg2Loc, regint_t *reg1Val, regint_t *reg2Val);

void writesubleqreg(uint8_t regLoc, regint_t regVal, bool regWrite);

void loadsubleqsubroutine(uint8_t opc, uint8_t funct);

void executesubleqsubroutine();

void setsubleqsource(regint_t src1, regint_t src2, bool regwrite);

regint_t getsubleqresult();

void executesubleqinstr();

void mipsexecution();

#endif