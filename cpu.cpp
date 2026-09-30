#include <iostream>
#include "cpu.hpp"

using namespace std;


Cpu::Cpu(){

   handler_table_.fill(&decode_nop);
   handler_table_[0b00100] = &decode_thumb16_movs_imm;

}

inline uint32_t Cpu::read_reg(Register reg) const{
  return regs_[static_cast<size_t>(reg)];
}

inline void Cpu::write_reg(Register reg, uint32_t value){ 
  regs_[static_cast<size_t>(reg)] = value;
}


// A bug here so when it is 32 bit instruction it will overwrite the first [15:0] bits instead of filling the rest 
// of the register
void Cpu::fetch16(Memory& memory){
  ir_ = static_cast<uint32_t>(memory.read_16bit(regs_[static_cast<size_t>(Register::PC)]));
}

void Cpu::increment_pc(uint32_t instruction_size){
  regs_[static_cast<size_t>(Register::PC)] += instruction_size;
}

void decode_instruction(DecodedInstruction& decoded){
  uint8_t handler_id = static_cast<uint8_t>((ir_ >> 10) & 0xff);

  if(handler_id >= 0b111010){
    // this is a 32 bit instruction
  }

  handler_table_[handler_id](decoded);
}

/* movs rd, imm - thumb 16-bit instruction */

void Cpu::decode_thumb16_movs_imm(DecodedInstruction& decoded){
  decoded.opcode = ir_ >> 11;
  decoded.imm = ir_ & 0xff;
  decoded.rn = static_cast<Register>((ir_ >> 8) & 0x7);
  decoded.execute = &execute_thumb16_movs_imm;
  decoded.instruction_size = 2;
}

void Cpu::execute_thumb16_movs_imm(const DecodedInstruction& decoded){
  write_reg(decoded.rd, decoded.imm);
}

/* No Operation - NOP instruction */

void decode_nop(DecodedInstruction& decoded){
  cout << "NOP - No Operation :D" << endl;
}

void cycle(Memory& memory){

  if(memory == nullptr){
    cout << "Memory class not initialized" << endl;
    return;
  }

  fetch16(memory);

  DecodedInstruction decoded_instr{};
  decode_instruction(decoded_instr);

  if(decoded_instr.execute == nullptr)
    cout << "No Operation; No assigned execution handler!" << endl;
    return;
  }

  decoded_instr.execute(decoded_instr);

  increment_pc(decoded_instr.size);

}

/*
 * I have an instruction : 0x2003 movs r0, #3
 *
 * 0010 0000 0000 0011; to get the opcode, shift >> 10 & 0x1f; this becomes 0000 0000 0000 1000
 * using that number, I use that as an index so instr_arr[0b00100] = &movs_instr_handler.
 *
 * Here it 
 */


