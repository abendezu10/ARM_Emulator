#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <unordered_map>

#define SIGN(x) ((x >> 31U) & 0x1U)

class Memory;

class Cpu{
  public:

    static constexpr uint32_t NEGATIVE_FLAG_MASK   = 1U << 31; /* N flag */
    static constexpr uint32_t ZERO_FLAG_MASK       = 1U << 30; /* Z flag */
    static constexpr uint32_t CARRY_FLAG_MASK      = 1U << 29; /* C flag */
    static constexpr uint32_t OVERFLOW_FLAG_MASK   = 1U << 28; /* V flag */
    static constexpr uint32_t SATURATION_FLAG_MASK = 1U << 27; /* Q flag */

    static constexpr std::array<std::string_view, 16> register_names = {
      "r0", "r1", "r2", "r3",
      "r4", "r5", "r6", "r7",
      "r8", "r9", "r10", "r11",
      "r12", "sp", "lr", "pc"
    };

    enum class Register : std::uint8_t{
      R0, R1, R2, R3, R4, R5, R6, R7,
      R8, R9, R10, R11, R12, SP, LR, PC
    };

    struct DecodedInstruction;

    using DecodeHandler = void (Cpu::*)(DecodedInstruction&);
    using ExecuteHandler = void (Cpu::*)(const DecodedInstruction&, Memory&);

    
    struct DecodedInstruction{
      std::uint32_t opcode;
      std::int32_t imm;
      std::uint16_t reg_list;
      std::string instr;
      Register rd;
      Register rm;
      Register rn;
      ExecuteHandler execute;
      std::size_t size; // instruction_size in bytes
    };

    explicit Cpu();
    
    inline uint32_t read_reg(Register reg) const{
      return regs_[static_cast<size_t>(reg)];
    }

    inline bool read_negative_flag() const{
      return flags_ & NEGATIVE_FLAG_MASK;
    }

    inline bool read_zero_flag() const{
      return flags_ & ZERO_FLAG_MASK;
    }

    inline bool read_carry_flag() const{
      return flags_ & CARRY_FLAG_MASK;
    }

    inline bool read_overflow_flag() const{
      return flags_ & OVERFLOW_FLAG_MASK; 
    }

    inline bool read_saturation_flag() const{
      return flags_ & SATURATION_FLAG_MASK;
    }

    void cycle(Memory& memory);

    void reset(Memory& memory);


  private:
    uint32_t flags_{};
    std::array<DecodeHandler, 64> handler_table_{};
    std::array<uint32_t, 16> regs_{};
    uint32_t ir_{0};

    inline void write_reg(Register reg, uint32_t value){ 
    regs_[static_cast<size_t>(reg)] = value;
    }

    inline void set_negative_flag(bool value){
      if(value)
        flags_ |= NEGATIVE_FLAG_MASK;
      else
        flags_ &= ~NEGATIVE_FLAG_MASK;
    }

    inline void set_zero_flag(bool value){
      if(value)
        flags_ |= ZERO_FLAG_MASK;
      else
        flags_ &= ~ZERO_FLAG_MASK;
    }

    inline void set_carry_flag(bool value){
      if(value)
        flags_ |= CARRY_FLAG_MASK;
      else
        flags_ &= ~CARRY_FLAG_MASK;
    }

    inline void set_overflow_flag(bool value){
      if(value)
        flags_ |= OVERFLOW_FLAG_MASK;
      else
        flags_ &= ~OVERFLOW_FLAG_MASK;
    }

    inline void set_sat_flag(bool value){
      if(value)
        flags_ |= SATURATION_FLAG_MASK;
      else
        flags_ |= ~SATURATION_FLAG_MASK;
    }
    
    void increment_pc(std::uint8_t instruction_size);

    void increment_sp(std::uint8_t bytes);
    void decrement_sp(std::uint8_t bytes);
    
    void fetch16(Memory& memory);
    void decode_instruction(DecodedInstruction& decoded, std::uint8_t handler_id);
    
    /* movs rd, imm */
    void decode_thumb16_movs_imm(DecodedInstruction& decoded);
    void execute_thumb16_movs_imm(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_ldr_literal(DecodedInstruction& decoded);
    void execute_thumb16_ldr_literal(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_str_word_imm(DecodedInstruction& decoded);
    void execute_thumb16_str_word_imm(const DecodedInstruction& decoded, Memory& memory);

    /* adds Rd, Ra, Rb | subs Rd, Ra, Rb - seems right!*/
    void decode_thumb16_adds_subs_reg_group(DecodedInstruction& decoded);
    void execute_thumb16_adds_regs(const DecodedInstruction& decoded, Memory& memory);
    void execute_thumb16_subs_regs(const DecodedInstruction& decoded, Memory& memory);

    /* adds Rd, #imm */
    void decode_thumb16_adds_imm(DecodedInstruction& decoded);
    void execute_thumb16_adds_imm(const DecodedInstruction& decoded, Memory& memory);
    /* subs Rd, #imm */
    void decode_thumb16_subs_imm(DecodedInstruction& decoded);
    void execute_thumb16_subs_imm(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_sp_add_sub_group(DecodedInstruction& decoded);
    void execute_thumb16_sp_add_imm(const DecodedInstruction& decoded, Memory& memory);
    void execute_thumb16_sp_sub_imm(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_data_processing_reg(DecodedInstruction& decoded);    
    void execute_thumb16_cmp_reg(const DecodedInstruction& decoded, Memory& memory);
    
    void decode_thumb16_add_rd_sp_imm(DecodedInstruction& decoded);
    void execute_thumb16_add_rd_sp_imm(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_special_data_processing_group(DecodedInstruction& decoded);
    void execute_thumb16_special_add_reg(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_uncond_branch(DecodedInstruction& decoded);
    void execute_thumb16_uncond_branch(const DecodedInstruction& decoded, Memory& memory);
    
    void decode_thumb16_push_misc_group(DecodedInstruction& decoded);
    void execute_thumb16_push(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_ldr_word_imm(DecodedInstruction& decoded);
    void execute_thumb16_ldr_word_imm(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb16_cond_branch_group(DecodedInstruction& decoded);
    void execute_thumb16_cond_branch_beq(const DecodedInstruction& decoded, Memory& memory);
    void execute_thumb16_cond_branch_bne(const DecodedInstruction& decoded, Memory& memory);
    void execute_thumb16_cond_branch_bcs(const DecodedInstruction& decoded, Memory& memory);
    void execute_thumb16_cond_branch_bcc(const DecodedInstruction& decoded, Memory& memory);

    void decode_thumb32_branch_group(DecodedInstruction& decoded);
    void execute_thumb32_branch_bl(const DecodedInstruction& decoded, Memory& memory);
    
    void decode_nop(DecodedInstruction& decoded);

    void cpu_state(void) const;
};


