#include "entrypoint.h"

int main(void) {
    mipsexecution();
	for (int i = 0; i < 8; i++) {
        int data = readDMM(MEM_SIZE-(4*i), true);
		printf("%d, ", data);
	}

    return 0;
}