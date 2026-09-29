#pragma once

#include <array>
#include <cstdint>
#include <functional>

class Cpu{
  private:
  // General Purpose Registers
    using DecodeHandler = DecodedInstruction (*)();
    using ExecuteHandler = void (*)(uint32_t, uint32_t, uint32_t, int32_t);

    std::array<DecodeHandler, 64> handler_table_{};
    std::array<uint32_t, 16> regs_{};
    uint32_t ir_{0};

    void increment_pc_(uint8_t instruction_size);


  public:

    explicit Cpu();

    enum class Register : uint8_t{
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

    struct DecodedInstruction{
      uint32_t opcode;
      uint16_t imm;
      Register rd;
      Register rn;
      ExecuteHandler execute;
      uint8_t instruction_size;
    }

    uint32_t read_reg(Register reg) const;
    void write_reg(Register reg, uint32_t value);
    
    DecodedInstruction decode_movs_handler();

    void execute_movs_handler();






  
};


