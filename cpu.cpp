#include <iostream>
#include "cpu.hpp"

using namespace std;

uint32_t Cpu::read_reg(Register reg) const{
  auto index = static_cast<size_t>(reg);
  return regs_[index];
}

void Cpu::increment_pc(uint32_t instruction_size){
  regs_[static_cast<size_t>(Register::PC)] += instruction_size;
}

void Cpu::write_reg(Register reg, uint32_t value){ 
  regs_[static_cast<size_t>(Register::reg)] = value;
}

