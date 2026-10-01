#include "memory.hpp"
#include "bin_file.hpp"

using namespace std;

Memory::Memory(BinFile& binfile){

  uintmax_t size = binfile.get_file_size();

  while(size--){
    int current_byte = binfile.get_byte();

    if(current_byte == EOF)
      break;

    memory_.push_back(current_byte);
  }
}

uint8_t Memory::read_8bit(uint32_t addr){
  return memory_[addr];
}

uint16_t Memory::read_16bit(uint32_t addr){
  return (static_cast<uint16_t>(memory_[addr])) | 
         (static_cast<uint16_t>(memory_[addr + 1]) << 8);
}

uint32_t Memory::read_32bit(uint32_t addr){
  return (static_cast<uint32_t>(memory_[addr])) |
         ((static_cast<uint32_t>(memory_[addr + 1])) << 8)  |
         ((static_cast<uint32_t>(memory_[addr + 2])) << 16) |
         ((static_cast<uint32_t>(memory_[addr + 3])) << 24);
}

void Memory::write_8bit(uint32_t addr, uint8_t value){
  memory_[addr] = value;
}

void Memory::write_16bit(uint32_t addr, uint16_t value){
  memory_[addr] = static_cast<uint8_t>(value);
  memory_[addr] = static_cast<uint8_t>(value >> 8);
}

void Memory::write_32bit(uint32_t addr, uint32_t value){
  memory_[addr] = static_cast<uint8_t>(value);
  memory_[addr] = static_cast<uint8_t>(value >> 8);
  memory_[addr] = static_cast<uint8_t>(value >> 16);
  memory_[addr] = static_cast<uint8_t>(value >> 24);
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
