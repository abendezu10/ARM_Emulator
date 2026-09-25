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

  enum class Opcode{
    movs,
    mov ,
    add ,
    adds
  };

  struct decoded_instr{
    Opcode opcode;
    uint32_t dest_reg;
    uint32_t src_reg;
    uint32_t imm_value;
    uint8_t instr_size;
  }

    uint32_t read_reg(Register reg) const;
    void write_reg(Register reg, uint32_t value);

  
};


