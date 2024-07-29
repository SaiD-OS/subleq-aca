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
gcc -o {{executablefilename}}.exe|sh .\architecture\arch.c
```
To run the final code execute the following command
```
{{executablefilename}}.exe
```

To build using make, first comment the defined ARCH and BENCHMARK variable in config.h and run following commands
```
make clean
make dir
make all -e AVAL=<Architecture type> -e BVAL=<Benchmark type>
make run
```

Architecture type
1 - Base Subleq
2 - Subleq + Mul
3 - Subleq + Right Shift
4 - Subleq + Reversal Subleq
5 - MIPS

Benchmark type - A number from 0 to 17
