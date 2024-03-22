//reg 0 to reg 3 values are constants and cannot be editted
//reg 4 is SRC1
//reg 5 is SRC2
//reg 6 is DEST
//rest are temporary registers
#define SZERO 0
#define SMONE 1
#define SONE 2
#define SWORD 3
#define SRC1 4
#define SRC2 5
#define DEST 6
regint_t subleqreg[SUBLEQREG_SIZE] = {0, -1 , 1, 32, 0, 0, 0, 0, 0};