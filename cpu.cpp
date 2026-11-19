
/* Since we are going to add FLASH and SRAM, we are going to need to add the memory architecture
 * in order for the instructions like "push" and "ldr/str" to work.
 */

#include <iostream>
#include <bitset>
#include <cstdint>
#include <cstddef>
#include <stdexcept>

#include "cpu.hpp"
#include "memory.hpp"

namespace{
  
  void debug_access(std::string asm_instr,
                    std::size_t size
                   )
  {
    std::cout
    << "[CPU]"
    << " DECODE "
    << size
    << "-bit"
    << " | "
    << asm_instr
    << std::endl;
  }

  

}

Cpu::Cpu(){
  
  handler_table_.fill(&Cpu::decode_nop);
  this->handler_table_[0b001000] = &Cpu::decode_thumb16_movs_imm; // 5 opcode bits
  this->handler_table_[0b001001] = &Cpu::decode_thumb16_movs_imm; // 5 opcode bits
  this->handler_table_[0b010010] = &Cpu::decode_thumb16_ldr_literal;
  this->handler_table_[0b010011] = &Cpu::decode_thumb16_ldr_literal;
  this->handler_table_[0b000110] = &Cpu::decode_thumb16_adds_subs_reg_group;
  this->handler_table_[0b000111] = &Cpu::decode_thumb16_adds_subs_reg_group;
  this->handler_table_[0b001100] = &Cpu::decode_thumb16_adds_imm;
  this->handler_table_[0b001101] = &Cpu::decode_thumb16_adds_imm;
  this->handler_table_[0b001110] = &Cpu::decode_thumb16_subs_imm;
  this->handler_table_[0b001111] = &Cpu::decode_thumb16_subs_imm;
  this->handler_table_[0b111000] = &Cpu::decode_thumb16_uncond_branch; // 5 opcode bits
  this->handler_table_[0b111001] = &Cpu::decode_thumb16_uncond_branch; // 5 opcode bits
  this->handler_table_[0b101101] = &Cpu::decode_thumb16_push_misc_group; // 7 opcode bits
  this->handler_table_[0b010000] = &Cpu::decode_thumb16_data_processing_reg;
;
}

void Cpu::increment_pc(std::uint8_t instruction_size){
  regs_[static_cast<std::size_t>(Register::PC)] += instruction_size;
}

void Cpu::increment_sp(std::uint8_t bytes){
  regs_[static_cast<std::size_t>(Register::SP)] += bytes;
}

void Cpu::decrement_sp(std::uint8_t bytes){
  regs_[static_cast<std::size_t>(Register::SP)] -= bytes;
}

// A bug here so when it is 32 bit instruction it will overwrite the first [15:0] bits instead of filling the rest
// of the register
void Cpu::fetch16(Memory& memory){
  ir_ = static_cast<std::uint32_t>(memory.read_16bit(regs_[static_cast<std::size_t>(Register::PC)]));
  increment_pc(2);
}

void Cpu::reset(Memory& memory){
  regs_[static_cast<std::size_t>(Register::SP)] = memory.read_32bit(0); // stores first val in vector table to SP
  std::uint32_t reset_hand = memory.read_32bit(4); // retrieves reset_hand & 1

  if(!(reset_hand & 0x1)){
    throw std::runtime_error("thumb state is not on!");
  }
  regs_[static_cast<std::size_t>(Register::PC)] = reset_hand & (~1);
  cpu_state();
}


void Cpu::cycle(Memory& memory){
  
  Cpu::DecodedInstruction decoded_instr{};

  fetch16(memory);
  //wrong
  
  std::uint8_t handler_id = static_cast<std::uint8_t>((ir_ >> 10) & 0x3f);

  if(handler_id >= 0b111010){
    // this is a 32 bit instruction and first it fetches 16 bit then fetches other half word if it has this prefix
    ir_ |= static_cast<std::uint32_t>(memory.read_16bit(regs_[static_cast<std::size_t>(Register::PC)])) << 16;
    increment_pc(2);
    // A 32 bit instr is laid out on memory as follow AB CD EF GH, then when fetched into ir_ it needs to be 
    // GH EF CD AB 
  }

  decode_instruction(decoded_instr, handler_id);

  if(decoded_instr.execute == nullptr){
    std::cout << "No Operation; No assigned execution handler!" << std::endl;
    return;
  }
  debug_access(decoded_instr.instr, decoded_instr.size);

  ExecuteHandler execute = decoded_instr.execute;
  (this->*execute)(decoded_instr, memory);

  cpu_state();
}

void Cpu::decode_instruction(DecodedInstruction& decoded, uint8_t handler_id){

  DecodeHandler handler = this->handler_table_[handler_id];
  (this->*handler)(decoded);
}

/* movs rd, imm - thumb 16-bit instruction */
void Cpu::decode_thumb16_movs_imm(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 11) & 0x1f;
  decoded.instr = "movs Rd, imm8";

  decoded.imm = ir_ & 0xff;
  decoded.rd = static_cast<Register>((ir_ >> 8) & 0x7);
  decoded.execute = &Cpu::execute_thumb16_movs_imm;
  decoded.size = 16;
}

void Cpu::execute_thumb16_movs_imm(const DecodedInstruction& decoded, Memory& memory){
  write_reg(decoded.rd, decoded.imm);
}

void Cpu::decode_thumb16_ldr_literal(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 11) & 0x1f;
  decoded.instr = "ldr Rt, imm8";
  decoded.imm = ir_ & 0xff;
  decoded.rd = static_cast<Register>((ir_ >> 8) & 0x7);
  decoded.execute = &Cpu::execute_thumb16_ldr_literal;
  decoded.size = 16;
}

void Cpu::execute_thumb16_ldr_literal(const DecodedInstruction& decoded, Memory& memory){
  uint32_t temp_pc = regs_[static_cast<size_t>(Register::PC)] + 2;
  /* alignment operation */
  temp_pc &= ~0x3U;
  temp_pc += decoded.imm * 4;
  write_reg(decoded.rd, memory.read_32bit(temp_pc));
}

/* adds Rd, Rn, Rm */ 
void Cpu::decode_thumb16_adds_subs_reg_group(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 9) & 1;
  decoded.instr = "adds Rd, Rn, Rm";
  decoded.rd = static_cast<Register>(ir_ & 0x7U);
  decoded.rn = static_cast<Register>((ir_ >> 3U) & 0x7U);
  decoded.rm = static_cast<Register>((ir_ >> 6U) & 0x7U);
  decoded.size = 16;

  switch(decoded.opcode){
    case 0:
      decoded.execute = &Cpu::execute_thumb16_adds_regs;
      break;

    case 1:
      decoded.execute = &Cpu::execute_thumb16_subs_regs;
      break;
  };
}

void Cpu::execute_thumb16_adds_regs(const DecodedInstruction& decoded, Memory& memory){
  const std::uint32_t rn = read_reg(decoded.rn);
  const std::uint32_t rm = read_reg(decoded.rm);

  const std::uint64_t wide_sum = static_cast<std::uint64_t>(rn) + static_cast<std::uint64_t>(rm);
  const std::uint32_t result = static_cast<std::uint32_t>(wide_sum);

  set_carry_flag((wide_sum >> 32U) != 0U);
  set_zero_flag(result == 0U);
  set_negative_flag(SIGN(result) != 0U);

  const bool overflow = (SIGN(rn) == SIGN(rm)) && (SIGN(result) != SIGN(rn));
  set_overflow_flag(overflow);

  write_reg(decoded.rd, result);
}

void Cpu::execute_thumb16_subs_regs(const DecodedInstruction& decoded, Memory& memory){
  write_reg(decoded.rd, read_reg(decoded.rn) - read_reg(decoded.rm));
}

void Cpu::decode_thumb16_adds_imm(DecodedInstruction& decoded){

  decoded.opcode = (ir_ >> 11) & 0x1f;
  decoded.instr = "adds Rd, imm8";
  decoded.imm = ir_ & 0xff;
  decoded.rd = static_cast<Register>((ir_ >> 8)& 0x7);
  decoded.execute = &Cpu::execute_thumb16_adds_imm;
  decoded.size = 16;
}

void Cpu::execute_thumb16_adds_imm(const DecodedInstruction& decoded, Memory& memory){
  write_reg(decoded.rd, read_reg(decoded.rd) + decoded.imm);
}

void Cpu::decode_thumb16_subs_imm(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 11) & 0x1f;
  decoded.instr = "subs Rd, imm8";
  decoded.imm = ir_ & 0xff;
  decoded.rd = static_cast<Register>((ir_ >> 8) & 0x7);
  decoded.execute = &Cpu::execute_thumb16_subs_imm;
  decoded.size = 16;
}

void Cpu::execute_thumb16_subs_imm(const DecodedInstruction& decoded, Memory& memory){
  write_reg(decoded.rd, read_reg(decoded.rd) + decoded.imm);
}



// B label: 11100 | imm11, where branch offset = sign_extend(imm11 << 1)
void Cpu::decode_thumb16_uncond_branch(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 11) & 0x1f;
  decoded.instr = "B label";
  decoded.imm = ir_ & 0x7ff;
  decoded.imm <<= 1;
  decoded.execute = &Cpu::execute_thumb16_uncond_branch;
  decoded.size = 16;
}

void Cpu::execute_thumb16_uncond_branch(const DecodedInstruction& decoded, Memory& memory){
  /* Based on CPU pipeline of the ARM architecture, the hardware_pc and emulator_pc (apart of the regs_)
   * are different values so i just used a simple hardware_pc to simulate that
   */

  std::cout << "Unconditional Jump Instruction" << std::endl;
  std::uint32_t hardware_pc = read_reg(Register::PC) + 2;
  write_reg(Register::PC, hardware_pc + static_cast<std::uint32_t>(decoded.imm));
}

void Cpu::decode_thumb16_push_misc_group(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 9) & 1;
  decoded.instr = "push <registers>";
  decoded.reg_list &= 0;
  decoded.reg_list |= (ir_ & (1 << 14));
  decoded.reg_list |= static_cast<std::uint16_t>((ir_ & 0xff));
  decoded.size = 16;

  switch(decoded.opcode){
    case 0:
      decoded.execute = &Cpu::execute_thumb16_push;
      break;

    case 1:
      std::cout << "Undefined!" << std::endl;
      break;
  };
}

//complete push
void Cpu::execute_thumb16_push(const DecodedInstruction& decoded, Memory& memory){

  if((decoded.reg_list >> 14) & 1){
    memory.write_32bit(regs_[static_cast<std::size_t>(Register::SP)], regs_[static_cast<std::size_t>(Register::LR)]);
    decrement_sp(4);
  }

  for(int i = 7; i >=0; i--){
    std::uint8_t push_reg = (decoded.reg_list >> i) & 0x1;
    if(push_reg){
      memory.write_32bit(regs_[static_cast<size_t>(Register::SP)], regs_[i]);
      decrement_sp(4);
    }
  }
}


void Cpu::decode_thumb16_data_processing_reg(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 9) & 0x3fU;
  decoded.rm = static_cast<Register>((ir_ >> 3) & 0x7U);
  decoded.rn = static_cast<Register>(ir_ & 0x7U);
  decoded.size = 16;
  uint8_t op = (ir_ >> 6) & 0xfU;

  switch (op) {
    case 0b1010 : 
      decoded.instr = "cmp Rn, Rm";
      decoded.execute = &Cpu::execute_thumb16_data_processing_cmp;
      break;
      
    default: 
      break;
  }

}

void Cpu::execute_thumb16_data_processing_cmp(const DecodedInstruction& decoded, Memory& memory){
  const std::uint32_t rn = read_reg(decoded.rn);
  const std::uint32_t rm = read_reg(decoded.rm);
  const std::uint32_t result = rn - rm;

  set_negative_flag((result >> 31U) != 0U);
  set_zero_flag(result == 0U);
  set_carry_flag(rn >= rm);

  const bool overflow = (SIGN(rn) != SIGN(rm)) && (SIGN(result) != SIGN(rn));

  set_overflow_flag(overflow);
}

/* No Operation - NOP instruction */

void Cpu::decode_nop(DecodedInstruction& decoded){
  decoded.instr = "nop";
  decoded.size = 32;
}














void Cpu::cpu_state() const {

    auto print_reg = [this](const char* name, Register reg) {
        std::cout
            << std::right
            << std::setw(3)
            << std::setfill(' ')
            << name
            << " 0x"
            << std::hex
            << std::setw(8)
            << std::setfill('0')
            << read_reg(reg)
            << std::setfill(' ');
    };

    std::cout
        << "┌───────────────────────────────────┐\n"
        << "│             CPU STATE             │\n"
        << "├───────────────────────────────────┤\n";

    std::cout << "│ ";
    print_reg("PC", Register::PC);
    std::cout << "    ";
    print_reg("SP", Register::SP);
    std::cout << "  │\n";

    std::cout << "│ ";
    print_reg("LR", Register::LR);
    std::cout << "\t\t    │\n";

    std::cout
        << "├───────────────────────────────────┤\n";

    std::cout << "│ ";
    print_reg("R0", Register::R0);
    std::cout << "     ";
    print_reg("R1", Register::R1);
    std::cout << " │\n";

    std::cout << "│ ";
    print_reg("R2", Register::R2);
    std::cout << "     ";
    print_reg("R3", Register::R3);
    std::cout << " │\n";

    std::cout << "│ ";
    print_reg("R4", Register::R4);
    std::cout << "     ";
    print_reg("R5", Register::R5);
    std::cout << " │\n";

    std::cout << "│ ";
    print_reg("R6", Register::R6);
    std::cout << "     ";
    print_reg("R7", Register::R7);
    std::cout << " │\n";

    std::cout << "│ ";
    print_reg("R8", Register::R8);
    std::cout << "     ";
    print_reg("R9", Register::R9);
    std::cout << " │\n";

    std::cout << "│ ";
    print_reg("R10", Register::R10);
    std::cout << "     ";
    print_reg("R11", Register::R11);
    std::cout << " │\n";

    std::cout << "│ ";
    print_reg("R12", Register::R12);
    std::cout << "\t\t    │\n";

    std::cout
        << "├───────────────────────────────────┤\n";

    // Add flags here, for example:
    std::cout << "│ N = " << read_negative_flag()
              << "  Z = " << read_zero_flag()
              << "  C = " << read_carry_flag()
              << "  V = " << read_overflow_flag()
              << "  Q = " << read_saturation_flag()
              << " │\n";

    std::cout
        << "└───────────────────────────────────┘\n"
        << std::dec
        << std::setfill(' ');
}

/*
 * I have an instruction : 0x2003 movs r0, #3
 *
 * 0010 0000 0000 0011; to get the opcode, shift >> 10 & 0x1f; this becomes 0000 0000 0000 1000
 * using that number, I use that as an index so instr_arr[0b00100] = &movs_instr_handler.
 *
 * Here it
 */
