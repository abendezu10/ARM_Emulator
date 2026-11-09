#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include <cstdio>

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

Memory::Memory(BinFile& binfile){

  std::uintmax_t size = binfile.get_file_size();

  int index = 0;

  while(size--){
    int current_byte = binfile.get_byte();

    if(current_byte == EOF)
      break;
    flash_.write_8bit(index, current_byte);
    index++;

  }
}

std::uint8_t Memory::read_8bit(std::uint32_t addr){
  uint8_t byte = 0;
  if(addr <= 0x0807ffff && addr >= 0x08000000){
    // use flash_ object
    byte = flash_.read_8bit()
  }else if(addr >= 0x20000000 && addr <= 0x20017fff){
    // use sram object
    byte = sram_.read_8bit();
  } else 
    return -1;

  return byte;
}

std::uint16_t Memory::read_16bit(std::uint32_t addr){

  uint16_t instr = 0;
  
  if(addr <= 0x0807ffff && addr >= 0x08000000){
    // use flash_ object
    flash_.read_16bit();
  }else if(addr >= 0x20000000 && addr <= 0x20017fff){
    // use sram object
    sram_.read_16bit();
  }



  return (static_cast<std::uint16_t>(memory_[addr])) |
         (static_cast<std::uint16_t>(memory_[addr + 1]) << 8);
}

std::uint32_t Memory::read_32bit(std::uint32_t addr){

  if(addr == 0){
    return flash_.read_32bit(0x08000000);
  if(addr == 4){
    return flash_.read_32bit(0x08000004);
  }

  return 0;
}


  return (static_cast<std::uint32_t>(flash_.mem[addr])) |
         ((static_cast<std::uint32_t>(memory_[addr + 1])) << 8)  | 
         ((static_cast<std::uint32_t>(memory_[addr + 2])) << 16) |
         ((static_cast<std::uint32_t>(memory_[addr + 3])) << 24);
}

void Memory::write_8bit(std::uint32_t addr, std::uint8_t value){
  [addr] = value;
}

void Memory::write_16bit(std::uint32_t addr, std::uint16_t value){

  if(addr <= 0x0807ffff && addr >= 0x08000000){
    // use flash_ object
  }else if(addr >= 0x20000000 && addr <= 0x2001ffff){
    // use sram object
  }else 
    return;

}

void Memory::write_32bit(std::uint32_t addr, std::uint32_t value){

  

  memory_[addr] = static_cast<std::uint8_t>(value);
  memory_[addr] = static_cast<std::uint8_t>(value >> 8);
  memory_[addr] = static_cast<std::uint8_t>(value >> 16);
  memory_[addr] = static_cast<std::uint8_t>(value >> 24);
}

std::uint8_t Flash::read_8bit(std::uint32_t addr){
  uint32_t addr_i = addr - 0x08000000;

  return mem_[addr_i];
}

std::uint32_t Flash::read_32bit(std::uint32_t addr){
  
  uint32_t addr_i = addr - 0x08000000;

  return  (static_cast<uint32_t>(mem_[addr_i])) |
          ((static_cast<uint32_t>(mem_[addr_i])) << 8)  |
          ((static_cast<uint32_t>(mem_[addr_i])) << 16) |
          ((static_cast<uint32_t>(mem_[addr_i])) << 24); 
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
