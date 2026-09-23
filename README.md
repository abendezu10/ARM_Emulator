# ARM_Emulator

The ARM assembly code goes into the assembler and becomes binary. After that, it goes into the emulator, which I will show the change in registers and memory.

First, we have to make a simple CPU that is able to read the binary. Following fetch -> decode -> execute cycle. 

I'm building a C++ ARMv7-M Cortex-M emulator


The instruction layout follows:

    bits 15-9           bits 8-6        bits 5-3            bits 2-0
    opcode (7 bits)     Regm (3 bits)   Regn (3 bits)       Regd (3 bits) 
