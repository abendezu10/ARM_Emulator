#pragma once


class Cpu{
  private:
    // General Purpose Registers
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;

    uint32_t pc; // Program Counter
    uint32_t lr; // Link Register
    uint32_t sp; // Stack Pointer

  public:
    explicit Cpu();

  // need to create write in register and mayeb read register
};


