Implementation and Benchmarking an Area Efficient Two Instruction Set Computer - Subleq⊖

This project contains a full implementation of the MIPS Pipelined processor as the host processor, with coprocessor implementations for

1. Subleq Base
2. Subleq + Mul
3. Subleq + Right Shift
4. Subleq with bit reversal
5. No coprocessor only host

This can be configured by setting the variable ARCH in config.h


This project has a monitoring feature that will provide error, perf, info and debug level logging on execution of each instruction at each pipeline stage based upon the logging level configured by setting the variable LOGLEVEL in config.h

The benchmark to be run can be set by configuring the BENCHMARK variable in config.h

To build the project run the following command
```
gcc -o {{executablefilename}}.exe|sh .\architecture\arch.c .\monitor\monitor.c
```
To run the final code execute the following command
```
{{executablefilename}}.exe|sh
```
```
L1: SRC1, Z, L2     
L2: SRC2, z, L3
L3: DEST, DEST, L4
L4: Z, DEST, L5
L5: Z, Z, end
```
```
L1: SRC1, SRC2, L2
L2: DEST, DEST, L3
L3: SRC2, DEST, L4
L4: Z, Z end
```
```
L1: SRC1, TEMP2, L2
L2: DEST, DEST, L3
L3: TEMP2, DEST, L4
L4: SRC2, TEMP1, L5
L5: MONE, TEMP1, L8
L6: TEMP2, TEMP2, L7
L7: TEMP1, TEMP1, end
L8: TEMP2, DEST, L9
L9: TEMP2, TEMP2, L10
L10: DEST, TEMP2, L11
L11: Z, Z, L5
```
```
L1: DEST, DEST, L2
L2: SRC1, SRC2, L4
L3: MONE, DEST, end
L4: Z, Z, end
```
```
L1: DEST, DEST, L2
L2: Z, SRC1, L4
L3: Z, TEMP3, L6
L4: ONE, DEST, L5
L5: MONE, TEMP3, L6 
L6: SRC1, TEMP2, L7
L7: SRC2, Z, L8
L8: WORD, TEMP1, L9
L9: Z, TEMP1, L10
L10: MONE, TEMP1, L15
L11: TEMP2, TEMP2, L12
L12: Z, Z, L13
L13: TEMP3, TEMP3, L14
L14: TEMP1, TEMP1, end
L15: TEMP3, DEST, L16
L16: TEMP3, TEMP3, L17
L17: DEST, TEMP3, L18
L18: Z, SRC1, L23
L19: TEMP2, SRC1, L20
L20: TEMP2, TEMP2, L21
L21: SRC1, TEMP2, L22
L22: Z, Z, L10
L23: MONE, DEST, L24
L24: Z, Z, L19
```
```
L1: SRC1, TEMP2, L2
L2: DEST, DEST, L3
L3: TEMP2, DEST, L4
L4: SRC2, TEMP1, L5
L5: MONE, TEMP1, L8
L6: TEMP2, TEMP2, L7
L7: TEMP1, TEMP1, end
L8: TEMP2, DEST, L9
L9: Z, Z, L5
```
