#ifndef MONITOR_H
#define MONITOR_H

#include "../config/config.h"
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

#define NORMAL 0
#define IFSTAGE 1
#define IDSTAGE 2
#define EXESTAGE 3
#define MEMSTAGE 4
#define WBSTAGE 5

void loggernorm(uregint_t ir, uregint_t pc, regint_t *rf, int loglevel);

void loggerif(uregint_t ir, uregint_t pc, regint_t *rf, IFIDReg wrsv, EXMEMReg rdsv, int loglevel);

void loggerid(uregint_t ir, uregint_t pc, regint_t *rf, IDEXReg wrsv, IFIDReg rdsv, int loglevel);

void loggerex(uregint_t ir, uregint_t pc, regint_t *rf, EXMEMReg wrsv, IDEXReg rdsv, int loglevel);

void loggermem(uregint_t ir, uregint_t pc, regint_t *rf, MEMWBReg wrsv, EXMEMReg rdsv, int loglevel);

void loggerwb(uregint_t ir, uregint_t pc, regint_t *rf, IFIDReg wrsv, MEMWBReg rdsv, int loglevel);

void instructioninfo(uregint_t ir, regint_t *rf, char *stage);

void controlsignalsprint(uint8_t control, uint8_t printmask);

void logger(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb, int loglevel);

void logerr(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb);

void logperf(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb);

void loginfo(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb);

void logdebug(uregint_t ir, uregint_t pc, regint_t *rf, int stage, IFIDReg *ifid, IDEXReg *idex, EXMEMReg *exmem, MEMWBReg *memwb);

#endif