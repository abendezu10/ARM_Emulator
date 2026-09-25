#include <iostream>
#include "cpu.hpp"

using namespace std;

uint32_t Cpu::read_reg(Register reg) const{
  return regs_[static_cast<size_t>(reg)];
}

void Cpu::increment_pc(uint32_t instruction_size){
  regs_[static_cast<size_t>(Register::PC)] += instruction_size;
}



void Cpu::write_reg(Register reg, uint32_t value){ 
  regs_[static_cast<size_t>(reg)] = value;
}

uint16_t Cpu::fetch16(Memory& memory){
  uint16_t instr_16bit = memory.read_16bit(regs_[static_cast<size_t>(Register::PC)]);
  return instr_16bit;
}

void Cpu::decode(uint16_t half_word){
  uint8_t word_check = instr >> 11;

  if(word_check == 0x2b5d | word_check == 0x2b66 | word_check == 0x2267){
      
    return;
  }

  uint8_t opcode = (instr_16bit >> 12) & 0xf;
  uint8_t reg = (instr_16bit >> 8) & 0xf;
  uint8_t imm_value = instr_16bit & 0xff;
}

void Cpu::execute(){


}

void Cpu::step(){
  
}

