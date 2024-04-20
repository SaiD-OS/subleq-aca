#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//Log level 1 - error, 2 - performance, 3 - info, 4 - debug
#define LOGLEVEL 0

// This will give the detailed information on instructions used
#define MONITOR
// This will give data on the instructions per cycle, etc.
#define PROFILE
// Choose the architecture
// 1 - Base Subleq
// 2 - Subleq + Mul
// 3 - Subleq + Right Shift
// 4 - Subleq + Reversal Subleq
// 5 - MIPS
#define ARCH 1

#define SUBLEQB 1
#define SUBLEQM 2
#define SUBLEQRS 3
#define SUBLEQREV 4
#define MIPS 5

//Memory Specs
#define MEM_SIZE 1024
#define REG_SIZE 32
#define REG_NUMS 32
#define INCVAL (REG_SIZE/8)

//Subleq coprocessor specs
#define SUBLEQREG_SIZE 9

// 1 - Big endian
// 2 - Little endian
#define ENDIAN 1

#if REG_SIZE == 16
    typedef uint16_t uregint_t;
    typedef int16_t regint_t;
    typedef uint8_t immregint_t;
#elif REG_SIZE == 64
    typedef uint64_t uregint_t;
    typedef int64_t regint_t;
    typedef uint32_t immregint_t;
#else
    typedef uint32_t uregint_t;
    typedef int32_t regint_t;
    typedef int16_t immregint_t;
#endif

typedef uint8_t memunit;

//load particular benchmark code i.e. instruction memory
#define BENCHMARK 0

//Pipline stage write/read array index
#define PSWR 0
#define PSRD 1

typedef struct {
    uregint_t ir, pc;
} IFIDReg;

typedef struct {
    uregint_t ir, pc;
    regint_t regRs, regRt, signext; 
    uint8_t control, aluop, rt, rd;
} IDEXReg;

typedef struct {
    uregint_t ir;
    regint_t memData, aluOut, branchAddress; 
    uint8_t control, destReg;
} EXMEMReg;

typedef struct {
    uregint_t ir;
    regint_t memData, aluOut; 
    uint8_t control, destReg;
} MEMWBReg;

#endif