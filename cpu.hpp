#pragma once

#include <array>
#include <cstdint>


class Cpu{
  private:
  // General Purpose Registers
    std::array<uint32_t, 16> regs_{};
    void increment_pc(uint32_t instruction_size);

  public:
  enum class Register{
      R0,
      R1,
      R2,
      R3,
      R4,
      R5,
      R6,
      R7,
      R8,
      R9,
      R10,
      R11,
      R12,
      SP,
      LR,
      PC
    };

    uint32_t read_reg(Register reg) const;
    void write_reg(Register reg, uint32_t value);

  
};


