#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

class BinFile;

class Memory{
 
  private:
    std::vector<uint8_t> memory_;


  public:
    explicit Memory(BinFile& binfile);

    uint8_t  read_8bit(uint32_t addr);
    uint16_t read_16bit(uint32_t addr);
    uint32_t read_32bit(uint32_t addr);

    void write_8bit(uint32_t addr, uint8_t value);
    void write_16bit(uint32_t addr, uint16_t value);
    void write_32bit(uint32_t addr, uint32_t value);

    void print_memory();
};

