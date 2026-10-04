#pragma once
/*
 * 512 KiB of flash memory: 0x0800 0000 - 0x0807 ffff  uint8_t *flash = new uint8_t[524288]
 * 96 Kbytes of SRAM:       0x2000 0000 - 0x2001 7fff
*/


#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <memory>

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

    void print_memory() const;
};

class Flash : public Memory {
  public:
     

  private:
    std::unique_ptr<uint8_t> flash_ = std::make_unique<uint8_t>(524288);


};

class SRAM : public Memory {
  public:

  private:
    std::unique_ptr<uint8_t> sram_ = std::make_unique<uint8_t>(98304);




};
