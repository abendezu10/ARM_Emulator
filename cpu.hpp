#pragma once

#include <array>
#include <cstdint>
#include <functional>

class Cpu{
  private:
  // General Purpose Registers
    using instr_handler = DecodedInstruction (*)(uint16_t instruction);

    std::array<instr_handler, 64> handler_table_{}
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
      PC
    };

    struct DecodedInstruction{
      uint32_t opcode;
      uint32_t imm_value;
      uint32_t reg_dest;
      uint32_t reg_src;
    }

    uint32_t read_reg(Register reg) const;
    void write_reg(Register reg, uint32_t value);
    
    DecodedInstruction instr_movs_handler16()




  
};


