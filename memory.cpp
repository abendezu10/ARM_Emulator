#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <stdexecpt>

#include "memory.hpp"
#include "bin_file.hpp"

/*
 * 512 KiB of flash memory: 0x0800 0000 - 0x0807 ffff
 * 96 Kbytes of SRAM:       0x2000 0000 - 0x2001 7fff
 *
 *  0x0000 0000 is where the initial MSP is stored, and this value is stored inside of the MSP register
 *  0x0000 0004 is the address of the reset_handler, which handles copies to .data segment, zero initalizes to
 *  .bss segment, setups clocks and systems (SystemInit), and then goes to the main() function
 *
 */

std::uint8_t Flash::read_8bit(std::uint32_t offset){
  std::uint32_t addr_i = offset - 0x08000000;
  return mem_[addr_i];
}

void Flash::load_8bit(std::offset, std::uint8_t value){
  mem_[offset] = value;
}

std::size_t Flash::get_size(void) const{
  return used_size_;
}

void Flash::set_size(std::size_t size){
  used_size_ = size;
}

std::uint8_t SRAM::read_8bit(std::uint32_t offset){
  std::uint32_t addr_i = 0x2001ffff - offset;
  return mem_[addr_i];
}

void SRAM::write_8bit(std::uint32_t offset, std::uint8_t value){
  std::uint32_t addr_i = 0x2001ffff - offset;
  return mem_[addr_i];
}


Memory::Memory(BinFile& binfile){

  std::uintmax_t size = binfile.get_file_size();

  size_t index = 0;

  while(size--){
    int current_byte = binfile.get_byte();

    if(current_byte == EOF)
      break;
    flash_.load_8bit(index, current_byte);
    index++;
  }
  flash_.set_size(index);
  
}

std::uint8_t Memory::read_8bit(std::uint32_t addr){
  uint8_t byte = 0;
  if(addr <= 0x0807ffff && addr >= 0x08000000)
    byte = flash_.read_8bit(addr);
  else if(addr >= 0x20000000 && addr <= 0x20017fff)
    byte = sram_.read_8bit(addr);
  else 
    throw std::out_of_range("8-bit read is not in SRAM or Flash.");

  return byte;
}

std::uint16_t Memory::read_16bit(std::uint32_t addr){
  uint16_t halfword = 0;
  
  if(addr <= 0x0807ffff && addr >= 0x08000000){
    halfword = (static_cast<std::uint16_t>(flash_.read_8bit(addr + 2)) << 8) | 
               (static_cast<std::uint16_t>(flash_.read_8bit(addr)));
  }else if(addr >= 0x20000000 && addr <= 0x20017fff){
    // use sram object
    halfword = (static_cast<std::uint16_t>(flash_.read_8bit(addr + 2)) << 8) |
               (static_cast<std::uint16_t>(flash_.read_8bit(addr)));
  }else {
    throw std::out_of_range("16-bit read is not in SRAM or Flash.");
  }
  return halfword;
}
// strictly for simualting a microcontroller board reseting because arm arch never fetces 32 bit instruction, only
// two half words
std::uint32_t Memory::read_32bit(std::uint32_t addr){
  uint32_t fullword = 0;

  if(addr == 0){
    fullword = (static_cast<std::uint32_t>(flash_.read_8bit(0x08000003)) << 24) |
               (static_cast<std::uint32_t>(flash_.read_8bit(0x08000002)) << 16) |
               (static_cast<std::uint32_t>(flash_.read_8bit(0x08000001)) << 8)  |
               (static_cast<std::uint32_t>(flash_.read_8bit(0x08000000)));
  }else if(addr == 4){
    fullword = (static_cast<std::uint32_t>(flash_.read_8bit(0x08000007)) << 24) |
               (static_cast<std::uint32_t>(flash_.read_8bit(0x08000006)) << 16) |
               (static_cast<std::uint32_t>(flash_.read_8bit(0x08000005)) << 8)  |
               (static_cast<std::uint32_t>(flash_.read_8bit(0x08000004)));
  }else{
    throw std::invalid_argument("Using 32-bit read operation only is done for the first two entries in the vector table.");
  }
  return fullword;
}

void Memory::write_8bit(std::uint32_t addr, std::uint8_t value){
  
 if(addr >= 0x20000000 && addr <= 0x20017fff)
    sram_.write_8bit(addr, value);
 else 
   throw std::out_of_range("SRAM write address out of range for 8-bit value");
}

void Memory::write_16bit(std::uint32_t addr, std::uint16_t value){

  if(addr >= 0x20000000 && addr <= 0x20017fff)
    sram_.write_16bit(addr, value);
  else 
    throw std::out_of_range("SRAM write address out of range for 16-bit value");
}


void Memory::write_32bit(std::uint32_t addr, std::uint32_t value){
  if(addr >= 0x20000000 && addr <= 0x20017fff)
    sram_.write_32bit(addr, value);
  else
    throw std::out_of_range("SRAM write address out of range for 32-bit value");
}


void Memory::print_memory() const{
    std::cout << "The bytes are:\n";

    for (std::size_t i = 0; i < memory_.size(); ++i) {
        std::cout
            << "0x"
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<unsigned int>(memory_[i])
            << '\n';
    }

    std::cout << std::dec;
}
