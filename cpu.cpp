/* Since we are going to add FLASH and SRAM, we are going to need to add the memory architecture
 * in order for the instructions like "push" and "ldr/str" to work. 
 */

#include <iostream>
#include <bitset> 
#include <cstdint>  

#include "cpu.hpp"
#include "memory.hpp"

using namespace std;


Cpu::Cpu(){
  N_flag_ = 0;
  Z_flag_ = 0;
  C_flag_ = 0;
  V_flag_ = 0;
  Q_flag_ = 0;

  handler_table_.fill(&Cpu::decode_nop);
  this->handler_table_[0b001000] = &Cpu::decode_thumb16_movs_imm; // 5 opcode bits
  this->handler_table_[0b001001] = &Cpu::decode_thumb16_movs_imm; // 5 opcode bits
  this->handler_table_[0b000110] = &Cpu::decode_thumb16_add_sub_group; // 7 opcode bits
  this->handler_table_[0b111000] = &Cpu::decode_thumb16_uncond_branch; // 5 opcode bits
  this->handler_table_[0b111001] = &Cpu::decode_thumb16_uncond_branch; // 5 opcode bits
  this->handler_table_[0b101101] = &Cpu::decode_thumb16_push_misc_group; // 7 opcode bits

}

void Cpu::increment_pc(uint8_t instruction_size){
  regs_[static_cast<size_t>(Register::PC)] += instruction_size;
}

// A bug here so when it is 32 bit instruction it will overwrite the first [15:0] bits instead of filling the rest 
// of the register
void Cpu::fetch16(Memory& memory){
  ir_ = static_cast<uint32_t>(memory.read_16bit(regs_[static_cast<size_t>(Register::PC)]));
  cout << ir_ << endl;
  increment_pc(2);
}

void Cpu::cycle(Memory& memory){

  // if(memory == nullptr){
  //   cout << "Memory class not initialized" << endl;
  //   return;
  // }

  Cpu::DecodedInstruction decoded_instr{};

  fetch16(memory);

  decode_instruction(decoded_instr);

  if(decoded_instr.execute == nullptr){
    cout << "No Operation; No assigned execution handler!" << endl;
    return;
  }

  ExecuteHandler execute = decoded_instr.execute;
  (this->*execute)(decoded_instr);
}




void Cpu::decode_instruction(DecodedInstruction& decoded){
  uint8_t handler_id = static_cast<uint8_t>((ir_ >> 10) & 0x3f);
  cout << "Handler_id: " << bitset<8>(handler_id) << endl;

  if(handler_id >= 0b111010){
    // this is a 32 bit instruction
  }
  DecodeHandler handler = this->handler_table_[handler_id];
  (this->*handler)(decoded);
}

/* movs rd, imm - thumb 16-bit instruction */

void Cpu::decode_thumb16_movs_imm(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 11) & 0x1f;
  decoded.imm = ir_ & 0xff;
  decoded.rd = static_cast<Register>((ir_ >> 8) & 0x7);
  decoded.execute = &Cpu::execute_thumb16_movs_imm;
  decoded.size = 2;
}

void Cpu::execute_thumb16_movs_imm(const DecodedInstruction& decoded){
  cout << "MOVS instruction" << endl;
  write_reg(decoded.rd, decoded.imm);
}

void Cpu::decode_thumb16_add_sub_group(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 9) & 1;

  decoded.rd = static_cast<Register>(ir_ & 0xff); 
  decoded.rn = static_cast<Register>((ir_ >> 3) & 0xff);
  decoded.rm = static_cast<Register>((ir_ >> 6) & 0xff);
  decoded.size = 2;

  switch(decoded.opcode){
    case 0:
      decoded.execute = &Cpu::execute_thumb16_add_regs;
      break;

    case 1:
      decoded.execute = &Cpu::execute_thumb16_sub_regs;
      break;
  };
}

void Cpu::execute_thumb16_add_regs(const DecodedInstruction& decoded){
  cout << "ADDS instruction" << endl;
  uint64_t sum = static_cast<uint64_t>(read_reg(decoded.rm)) + static_cast<uint64_t>(read_reg(decoded.rn));
  if((sum >> 32) & 0x1U)
    set_carry_flag();
  else
    clear_carry_flag();
    
  write_reg(decoded.rd, sum);
}

void Cpu::execute_thumb16_sub_regs(const DecodedInstruction& decoded){
  cout << "SUBS instruction" << endl;
  write_reg(decoded.rd, read_reg(decoded.rn) - read_reg(decoded.rm));
}

void Cpu::decode_thumb16_uncond_branch(DecodedInstruction& decoded){

  decoded.opcode = (ir_ >> 11) & 0xff;
  
  int8_t offset = static_cast<int8_t>(ir_ & 0xff);
  decoded.imm = static_cast<int32_t>(offset) * 2;
  decoded.execute = &Cpu::execute_thumb16_uncond_branch;
  decoded.size = 2;
}

void Cpu::execute_thumb16_uncond_branch(const DecodedInstruction& decoded){
  /* Based on CPU pipeline of the ARM architecture, the hardware_pc and emulator_pc (apart of the regs_)
   * are different values so i just used a simple hardware_pc to simulate that 
   */

  cout << "Unconditional Jump Instruction" << endl;
  uint32_t hardware_pc = read_reg(Register::PC) + 2;
  write_reg(Register::PC, hardware_pc + static_cast<uint32_t>(decoded.imm));
}

void Cpu::decode_thumb16_push_misc_group(DecodedInstruction& decoded){
  decoded.opcode = (ir_ >> 9) & 1;
  
  switch(decoded.opcode){
    case 0:

      

      decoded.execute = &Cpu::execute_thumb16_push;
      break;

    case 1:
      break;
  };
}

void Cpu::execute_thumb16_push(const DecodedInstruction& decoded){

}




/* No Operation - NOP instruction */

void Cpu::decode_nop(DecodedInstruction& decoded){
  cout << "NOP - No Operation :D" << endl;
}


/*
 * I have an instruction : 0x2003 movs r0, #3
 *
 * 0010 0000 0000 0011; to get the opcode, shift >> 10 & 0x1f; this becomes 0000 0000 0000 1000
 * using that number, I use that as an index so instr_arr[0b00100] = &movs_instr_handler.
 *
 * Here it 
 */


