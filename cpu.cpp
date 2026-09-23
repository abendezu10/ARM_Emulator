#include <iostream>
#include "cpu.hpp"

using namespace std;

uint32_t Cpu::read_reg(Register reg){
  auto index = static_cast<std::size_t>(reg);
  return regs_[index];
}

void Cpu::write_reg(Register reg, uint32_t value){
  regs_[reg] = value;
}

