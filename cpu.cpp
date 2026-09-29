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

void Cpu::fetch16(Memory& memory){
  lr_ = memory.read_16bit(regs_[static_cast<size_t>(Register::PC)]);
}

void Cpu::increment_pc(uint32_t instruction_size){
  regs_[static_cast<size_t>(Register::PC)] += instruction_size;
}

DecodedInstruction decode(){
  struct DecodedInstruction decoded = {};
  uint8_t handler_id = static_cast<uint8_t>((ir_ >> 10) & 0xff);

  if(handler_id >= 0b111010){
    // this is a 32 bit instruction
  }

  decoded = handler_table_[handler_id]();

  return decoded_instr;
}

/* movs rd, imm - thumb 16-bit instruction */

DecodedInstruction Cpu::decode_thumb16_movs_imm(){
  struct DecodedInstruction decoded = {};

  decoded.opcode = ir_ >> 11;
  decoded.imm = ir_ & 0xff;
  decoded.rn = static_cast<Register>((ir_ >> 8) & 0x7);
  decoded.execute = &execute_thumb16_movs_imm;
  decode.instruction_size = 2;

  return decoded;
}

void Cpu::execute_thumb16_movs_imm(const DecodedInstruction& decoded){
  write_reg(decoded.rd, decoded.imm);
}

/* No Operation - NOP instruction */

DecodedInstruction decode_nop(){
  struct DecodedInstruction decoded = {};
  return decoded;
}

void cpu_cycle(Memory& memory){

  if(memory == nullptr){
    cout << "Memory class not initialized" << endl;
    return;
  }

  ir_ = static_cast<uint32_t>(fetch16(memory));

  struct DecodedInstruction decoded = decode();
  if(decoded.execute == nullptr){
    cout << "No Operation; No assigned execution handler!" << endl;
    return;
  }

  decode.execute(decoded);

  increment_pc(decoded.instruction_size);


}

/*
 * I have an instruction : 0x2003 movs r0, #3
 *
 * 0010 0000 0000 0011; to get the opcode, shift >> 10 & 0x1f; this becomes 0000 0000 0000 1000
 * using that number, I use that as an index so instr_arr[0b00100] = &movs_instr_handler.
 *
 * Here it 
 */


