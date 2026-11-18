#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <stdexcept>

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

namespace {

void debug_access(const char* operation,
                  const char* region,
                  std::uint32_t addr,
                  std::uint32_t value,
                  int bits)
{
  std::cout << "[MEM] "
      << operation
      << " " << bits << "-bit"
      << " | region=" << region
      << " | addr=0x"
      << std::hex
      << std::setw(8)
      << std::setfill('0')
      << addr
      << " | value=0x"
      << std::setw(bits / 4)
      << value
      << std::dec
      << std::setfill(' ')
      << std::endl;
}

}

void Flash::print_memory(void) const{
  for(size_t i = 0; i <= used_size_; i++){
    std::cout << "[FLASH] The byte at 0x" 
              << std::hex 
              << (0x08000000 + i)
              << ": 0x"
              << static_cast<unsigned int>(mem_[i])
              << "\n";
  }
}

std::uint8_t Flash::read_8bit(std::uint32_t offset){
  std::uint32_t addr_i = offset - 0x08000000;
  return mem_[addr_i];
}

std::uint16_t Flash::read_16bit(std::uint32_t offset){
  std::uint32_t addr_i = offset - 0x08000000;
  uint16_t halfword = (mem_[addr_i + 1] << 8) |
                      (mem_[addr_i]);
  return halfword;
}

std::uint32_t Flash::read_32bit(std::uint32_t offset){
  std::uint32_t addr_i = offset - 0x08000000;
  std::uint32_t fullword = (mem_[addr_i + 3] << 24) |
                      (mem_[addr_i + 2] << 16) |
                      (mem_[addr_i + 1] << 8)  |
                      (mem_[addr_i]);
  return fullword;
}

void Flash::load_8bit(std::uint32_t offset, std::uint8_t value){
  mem_[offset] = value;
}

std::size_t Flash::get_size(void) const{
  return used_size_;
}

void Flash::set_size(std::size_t size){
  used_size_ = size;
}

void SRAM::print_memory(void) const{
  for(std::size_t i; i < used_size_; i++){
    std::cout << "[SRAM] The byte at 0x" 
              << std::hex 
              << (0x20017fff - i)  
              << ": 0x" 
              << static_cast<unsigned int>(mem_[i]) 
              << "\n";
  }
}

std::uint8_t SRAM::read_8bit(std::uint32_t offset){
  std::uint32_t addr_i = 0x20017fff - offset;
  std::cout << "The 8-bit value is: 0x"
          << std::hex
          << static_cast<unsigned int>(mem_[addr_i])
          << std::dec
          << std::endl;

  return mem_[addr_i];
}

std::uint16_t SRAM::read_16bit(std::uint32_t offset){
  std::uint32_t addr_i = offset - 0x08000000;
  std::uint16_t halfword = (mem_[addr_i + 1] << 8) |
                      (mem_[addr_i]);
  std::cout << "The 8-bit value is: 0x"
          << std::hex
          << static_cast<unsigned int>(halfword)
          << std::dec
          << std::endl;
  return halfword;
}

std::uint32_t SRAM::read_32bit(std::uint32_t offset){
  std::uint32_t addr_i = 0x20017fff - offset;
  std::uint32_t fullword = (mem_[addr_i + 3] << 24) | 
                           (mem_[addr_i + 2] << 16) |
                           (mem_[addr_i + 1] << 8)  |
                           (mem_[addr_i]);
  std::cout << "The 8-bit value is: 0x"
          << std::hex
          << static_cast<unsigned int>(fullword)
          << std::dec
          << std::endl;
  return fullword;
}

void SRAM::write_8bit(std::uint32_t offset, std::uint8_t value){
  std::uint32_t addr_i = 0x20017fff - offset;
  mem_[addr_i] = value;
}

void SRAM::write_16bit(std::uint32_t offset, std::uint16_t value){
  std::uint32_t addr_i = 0x20017fff - offset;
  mem_[addr_i] = static_cast<std::uint8_t>((value >> 8) & 0xff);
  mem_[addr_i + 1] = static_cast<std::uint8_t>(value & 0xff);
}

void SRAM::write_32bit(std::uint32_t offset, std::uint32_t value){
  std::uint32_t addr_i = 0x20017fff - offset;

  mem_[addr_i] = static_cast<std::uint8_t>((value >> 24) & 0xff);
  mem_[addr_i + 1] = static_cast<std::uint8_t>((value >> 16) & 0xff);
  mem_[addr_i + 2] = static_cast<std::uint8_t>((value >> 8) & 0xff);
  mem_[addr_i + 3] = static_cast<std::uint8_t>(value & 0xff);
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

  std::cout
      << "[FLASH] Loaded "
      << index
      << " bytes";

  if(index > 0){
    std::cout
        << " | range=0x08000000-0x"
        << std::hex
        << (0x08000000 + index - 1)
        << std::dec;
  }

  std::cout << std::endl;

}

std::uint8_t Memory::read_8bit(std::uint32_t addr){
  uint8_t byte = 0;
  if(addr <= 0x0807ffff && addr >= 0x08000000){
    byte = flash_.read_8bit(addr);
    debug_access("READ", "FLASH", addr, byte, 8);
  }
  else if(addr >= 0x20000000 && addr <= 0x20017fff){
    byte = sram_.read_8bit(addr);
    debug_access("READ", "SRAM", addr, byte, 8);
  }
  else 
    throw std::out_of_range("8-bit read is not in SRAM or Flash.");

  return byte;
}

std::uint16_t Memory::read_16bit(std::uint32_t addr){
  uint16_t halfword = 0;

  if(addr <= 0x0807ffff && addr >= 0x08000000){
    halfword = flash_.read_16bit(addr);
    debug_access("READ", "FLASH", addr, halfword, 16);
  }else if(addr >= 0x20000000 && addr <= 0x20017fff){
    // use sram object
    halfword = sram_.read_16bit(addr);
    debug_access("READ", "SRAM", addr, halfword, 16);
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
    fullword = flash_.read_32bit(0x08000000);
    debug_access("RESET READ", "FLASH_ALIAS", addr, fullword, 32);
  }else if(addr == 4){
    fullword = flash_.read_32bit(0x08000004);
    debug_access("RESET READ", "FLASH_ALIAS", addr, fullword, 32);
  }else if(addr <= 0x0807ffff && addr >= 0x08000000){
    fullword = flash_.read_32bit(addr);
  } else {
    throw std::invalid_argument("Using 32-bit read operation only is done for the first two entries in the vector table.");
  }
  return fullword;
}

void Memory::write_8bit(std::uint32_t addr, std::uint8_t value){

 if(addr >= 0x20000000 && addr <= 0x20017fff){
    debug_access("WRITE", "SRAM", addr, value, 8);
    sram_.write_8bit(addr, value);
 }
 else 
   throw std::out_of_range("SRAM write address out of range for 8-bit value");
}

void Memory::write_16bit(std::uint32_t addr, std::uint16_t value){

  if(addr >= 0x20000000 && addr <= 0x20017fff){
    debug_access("WRITE", "SRAM", addr, value, 16);
    sram_.write_16bit(addr, value);
  }
  else 
    throw std::out_of_range("SRAM write address out of range for 16-bit value");
}


void Memory::write_32bit(std::uint32_t addr, std::uint32_t value){
  if(addr >= 0x20000000 && addr <= 0x20017fff){
    debug_access("WRITE", "SRAM", addr, value, 32);
    sram_.write_32bit(addr, value);
  }
  else
    throw std::out_of_range("SRAM write address out of range for 32-bit value");
}

std::size_t Memory::get_size(std::string region) const{
  std::size_t used_size = 0;
  if(region == "FLASH"){
    used_size = flash_.get_size();
  } else if (region == "SRAM"){
    used_size = 0;
  }
  return used_size;
}


void Memory::print_memory(std::string region) const{

  if(region == "FLASH"){
    // print flash memory
    flash_.print_memory();
  }else if(region == "SRAM"){
    sram_.print_memory();
  }
    // for (std::size_t i = 0; i < memory_.size(); ++i) {
    //     std::cout
    //         << "0x"
    //         << std::hex
    //         << std::setw(2)
    //         << std::setfill('0')
    //         << static_cast<unsigned int>(memory_[i])
    //         << '\n';
    // }

}
