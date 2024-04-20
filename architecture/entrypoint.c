#include "arch.h"

#ifdef PROFILE
unsigned int instructioncount = 0, rtype = 0, itype = 0, jtype = 0, arithmetic = 0, branch = 0;
#endif

int main(void) {
    mipsexecution();
	for (int i = 0; i < 8; i++) {
        int data = readDMM(MEM_SIZE-(4*i), true);
		printf("%d, ", data);
	}

    return 0;
}