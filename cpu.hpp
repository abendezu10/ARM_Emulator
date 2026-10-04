#pragma once

#include <array>
#include <cstdint>
#include <functional>

class Memory;

class Cpu{
  public:

    static constexpr uint32_t NEGATIVE_FLAG_MASK   = 1U << 31; /* N flag */
    static constexpr uint32_t ZERO_FLAG_MASK       = 1U << 30; /* Z flag */
    static constexpr uint32_t CARRY_FLAG_MASK      = 1U << 29; /* C flag */
    static constexpr uint32_t OVERFLOW_FLAG_MASK   = 1U << 28; /* V flag */
    static constexpr uint32_t SATURATION_FLAG_MASK = 1U << 27; /* Q flag */

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

    inline uint8_t read_negative_flag() const{
      return static_cast<uint8_t>(((flags_ & NEGATIVE_FLAG_MASK) >> 31));
    }

    inline uint8_t read_zero_flag() const{
      return static_cast<uint8_t>(((flags_ & ZERO_FLAG_MASK) >> 30));
    }

    inline uint8_t read_carry_flag() const{
      return static_cast<uint8_t>(((flags_ & CARRY_FLAG_MASK) >> 29));
    }

    inline uint8_t read_overflow_flag() const{
      return static_cast<uint8_t>(((flags_ & OVERFLOW_FLAG_MASK) >> 28));
    }

    inline uint8_t read_saturation_flag() const{
      return static_cast<uint8_t>(((flags_ & SATURATION_FLAG_MASK) >> 27));
    }

    void cycle(Memory& memory);


  private:
    uint32_t flags_;
    std::array<DecodeHandler, 64> handler_table_{};
    std::array<uint32_t, 16> regs_{};
    uint32_t ir_{0};

    inline void write_reg(Register reg, uint32_t value){ 
      regs_[static_cast<size_t>(reg)] = value;
    }

    inline void set_negative_flag(){
      flags_ |= NEGATIVE_FLAG_MASK;
    }

    inline void clear_negative_flag(){
      flags_ &= ~NEGATIVE_FLAG_MASK;
    }

    inline void set_zero_flag(){
      flags_ |= ZERO_FLAG_MASK;
    }

    inline void clear_zero_flag(){
      flags_ &= ~ZERO_FLAG_MASK;
    }

    inline void set_carry_flag(){
      flags_ |= CARRY_FLAG_MASK;
    }


    inline void clear_carry_flag(){
      flags_ &= ~CARRY_FLAG_MASK;
    }

    inline void set_overflow_flag(){
      flags_ |= OVERFLOW_FLAG_MASK;
    }

    inline void clear_overflow_flag(){
      flags_ &= ~OVERFLOW_FLAG_MASK; 
    }

    inline void set_sat_flag(){
      flags_ |= SATURATION_FLAG_MASK; 
    }

    inline void clear_sat_flag(){
      flags_ &= ~SATURATION_FLAG_MASK;
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

    void decode_thumb16_push_misc_group(DecodedInstruction& decoded);
    void execute_thumb16_push(const DecodedInstruction& decoded);

    void decode_nop(DecodedInstruction& decoded);


};


