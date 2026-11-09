#pragma once

/*
 * 512 KiB of flash memory: 0x0800 0000 - 0x0807 ffff
 * 96 Kbytes of SRAM:       0x2000 0000 - 0x2001 7fff
 */

#include <vector>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <memory>

class BinFile;

class Memory{
  public:
    explicit Memory(BinFile& binfile);

    std::uint8_t read_8bit(std::uint32_t addr);
    std::uint16_t read_16bit(std::uint32_t addr);
    std::uint32_t read_32bit(std::uint32_t addr);

    void write_8bit(std::uint32_t addr, std::uint8_t value);
    void write_16bit(std::uint32_t addr, std::uint16_t value);
    void write_32bit(std::uint32_t addr, std::uint32_t value);

    void print_memory() const;

    void write_8bit(std::uint32_t addr_i, std::uint8_t  value);

  private:
    Flash flash_;
    SRAM sram_;


};
  
class Flash{
  /*    
   * Even though flash addressing is 0, it starts at 0x08000000,
   */
  
  private:
    std::unique_ptr<std::uint8_t[]> mem_ = std::make_unique<std::uint8_t[]>(524288);

  public:
    std::uint8_t  read_8bit(std::uint32_t addr);
    std::uint16_t read_16bit(std::uint32_t addr);
    std::uint32_t read_32bit(std::uint32_t addr);

    void write_8bit(std::uint32_t addr, std::uint8_t value);
    void write_16bit(std::uint32_t addr, std::uint16_t value);
    void write_32bit(std::uint32_t addr, std::uint32_t value);

    void print_memory() const;

};

class SRAM{
  private:
    std::unique_ptr<std::uint8_t[]> mem_ = std::make_unique<std::uint8_t[]>(98304);

  public:
    std::uint8_t  read_8bit(std::uint32_t addr);
    std::uint16_t read_16bit(std::uint32_t addr);
    std::uint32_t read_32bit(std::uint32_t addr);

    void write_8bit(std::uint32_t addr, std::uint8_t value);
    void write_16bit(std::uint32_t addr, std::uint16_t value);
    void write_32bit(std::uint32_t addr, std::uint32_t value);

    void print_memory() const;
}


