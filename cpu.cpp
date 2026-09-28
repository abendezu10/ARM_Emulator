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

/*
 * I have an instruction : 0x2003 movs r0, #3
 *
 * 0010 0000 0000 0011; to get the opcode, shift >> 10 & 0x1f; this becomes 0000 0000 0000 1000
 * using that number, I use that as an index so instr_arr[0b00100] = &movs_instr_handler.
 *
 * Here it 
 */
