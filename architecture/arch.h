#ifndef ARCH_H
#define ARCH_H

#include "../config/config.h"
#include <stdbool.h>

#if BENCHMARK == 1
    #include "../benchmarks/adpcm.h"
#elif BENCHMARK == 2
    #include "../benchmarks/bf.h"
#elif BENCHMARK == 3
    #include "../benchmarks/bs.h"
#elif BENCHMARK == 4
    #include "../benchmarks/bubble.h"
#elif BENCHMARK == 5
    #include "../benchmarks/crc.h"
#elif BENCHMARK == 6
    #include "../benchmarks/fibcall.h"
#elif BENCHMARK == 7
    #include "../benchmarks/gsm.h"
#elif BENCHMARK == 8
    #include "../benchmarks/insertsort.h"
#elif BENCHMARK == 9
    #include "../benchmarks/intmm.h"
#elif BENCHMARK == 10
    #include "../benchmarks/jfdctint.h"
#elif BENCHMARK == 11
    #include "../benchmarks/matmul.h"
#elif BENCHMARK == 12
    #include "../benchmarks/mpeg2.h"
#elif BENCHMARK == 13
    #include "../benchmarks/pbmsrch_large.h"
#elif BENCHMARK == 14
    #include "../benchmarks/vec_add.h"
#elif BENCHMARK == 15
    #include "../benchmarks/bubblesort.h"
#elif BENCHMARK == 16
    #include "../benchmarks/matrixmul.h"
#elif BENCHMARK == 17
    #include "../benchmarks/shiftcompare.h"
#else
    #include "../benchmarks/fibo.h"
#endif

//Instruction
#define NOOP 0x00000000

//Memory Addresses
#define REGF_START 0x0
#define HI_LOC 0x21
#define LO_LOC 0x20
#define RETADDR_LOC 0x1F
#define V0_LOC 0x2
#define V1_LOC 0x3
#define A0_LOC 0x4
#define A1_LOC 0x5
#define A2_LOC 0x6
#define A3_LOC 0x7
#define SUBROUT_FIRST_INSTR 0x64
#define SUBROUT_ADD_START 0x64
#define SUBROUT_SUB_START 0x69
#define SUBROUT_SL_START 0x6D
#define SUBROUT_SLT_START 0x78
#define SUBROUT_SR_START 0x7C
#define SUBROUT_MULT_START 0x94
#define SUBROUT_LAST_INSTR 0x190
#define SUBROUT_MEM_OFFSET 0x22
#define SUBROUT_SZERO 0x0
#define SUBROUT_SMONE 0x1
#define SUBROUT_SONE 0x2
#define SUBROUT_SWORD 0x3
#define SUBROUT_SRC1 0x4
#define SUBROUT_SRC2 0x5
#define SUBROUT_DEST 0x6
#define PROG_START 0x800

//Defining Control Signals Masks for simplicity
//BYTE || HALF || SYS || MTCO || MFCO || FUNCTOP || UNSIGNED || JUMP || ZERO || MEMTOREG || ALUSRC || BRANCH || MEMREAD || MEMWRITE || REGWRITE || REGDEST
#define REGDESTM        0b00000001
#define REGWRITEM       0b00000010
#define MEMWRITEM       0b00000100
#define MEMREADM        0b00001000
#define BRANCHM         0b00010000
#define ALUSRCM         0b00100000
#define MEMTOREGM       0b01000000
#define ZEROM           0b10000000
#define JUMPM           0b100000000
#define UNSIGNEDM       0b1000000000
#define FUNCTOPM        0b10000000000
#define MFCOM           0b100000000000
#define MTCOM           0b1000000000000
#define SYSM            0b10000000000000
#define HALFM           0b100000000000000
#define BYTEM           0b1000000000000000

//BNE || BLTZAL || BLTZ || BLEZ || BGTZ || BGEZAL || BGEZ || BEQ
#define BEQM 0b00000001
#define BGEZM 0b00000010
#define BGEZALM 0b00000100
#define BGTZM 0b00001000
#define BLEZM 0b00010000
#define BLTZM 0b00100000
#define BLTZALM 0b01000000
#define BNEM 0b10000000

//Intruction Decode Masks
#define SHAMTM 0x0000001F
#define RTM 0x0000001F
#define RDM 0x0000001F
#define RSM 0x0000001F
#define FUNCTM 0x0000003F
#define IMMEDM 0x0000FFFF
#define JUMPFM 0x03FFFFFF

//Special Mask
#define JUMP_RPCM 0xF0000000

//Subleq instruction masks
#define SRC1REG 0x0000001F
#define SRC2REG 0x0000001F
#define BRANCHVAL 0x0000FFFF

#if defined(MONITOR) || defined(PROFILE)
bool signalParser(uint8_t control, uint8_t mask);

void controlsignalsprint(uint8_t control, uint8_t printmask);

void instructioninfo(uregint_t ir);
#endif

uregint_t incrementPC(uregint_t pc);

regint_t addBranchAddr(regint_t pc, regint_t branchAddr);

regint_t signExtn(immregint_t offset);

void generateAluControl(uint8_t aluop, uint8_t funct, uint8_t *alucontrol);

regint_t alu(regint_t op1, regint_t op2, uint8_t alucontrol, uint16_t *control);

void generateControlSignals(uregint_t instr, uint16_t *control, uint8_t *aluop, uint8_t *branchflags, uregint_t *executesubleqstart);

void mipsexecution();

#endif