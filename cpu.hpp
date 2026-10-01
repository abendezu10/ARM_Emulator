#pragma once

#include <array>
#include <cstdint>
#include <functional>

class Memory;

class Cpu{
  public:
    enum class Register : uint8_t{
      R0, R1, R2, R3, R4, R5, R6, R7,
      R8, R9, R10, R11, R12, SP, LR, PC
    };

    struct DecodedInstruction;

    using DecodeHandler = void (Cpu::*)(DecodedInstruction&);
    using ExecuteHandler = void (Cpu::*)(const DecodedInstruction& decoded);

    
    struct DecodedInstruction{
      uint32_t opcode;
      int16_t imm;
      Register rd;
      Register rm;
      Register rn;
      ExecuteHandler execute;
      uint8_t size;
    };
    explicit Cpu();
    
    inline uint32_t read_reg(Register reg) const{
      return regs_[static_cast<size_t>(reg)];
    }


    void cycle(Memory& memory);


  private:
    std::array<DecodeHandler, 64> handler_table_{};
    std::array<uint32_t, 16> regs_{};
    uint32_t ir_{0};

    inline void write_reg(Register reg, uint32_t value){ 
      regs_[static_cast<size_t>(reg)] = value;
    }
    
    void increment_pc(uint8_t instruction_size);
    
    void fetch16(Memory& memory);
    void decode_instruction(DecodedInstruction& decoded);
    
    void decode_thumb16_movs_imm(DecodedInstruction& decoded);
    void execute_thumb16_movs_imm(const DecodedInstruction& decoded);

    void decode_thumb16_add_sub_group(DecodedInstruction& decoded);
    void execute_thumb16_add_regs(const DecodedInstruction& decoded);
    void execute_thumb16_sub_regs(const DecodedInstruction& decoded);

    void decode_thumb16_uncond_branch(DecodedInstruction& decoded);
    void execute_thumb16_uncond_branch(const DecodedInstruction& decoded);

    void decode_nop(DecodedInstruction& decoded);


};


