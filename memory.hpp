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

class Flash{
  /*    
   * Even though flash addressing is 0, it starts at 0x08000000,
   */
  
  private:
    static constexpr std::size_t CAPACITY = 524288;

    std::unique_ptr<std::uint8_t[]> mem_ = std::make_unique<std::uint8_t[]>(CAPACITY);
    std::size_t used_size_ = 0;

  public:
    std::uint8_t  read_8bit(std::uint32_t offset);
    std::uint8_t load_8bit(std::uint32_t offset);
    std::size_t get_size(void);
    std::size_t set_size(void);
    void print_memory() const;

};

class SRAM{
  private:
    std::unique_ptr<std::uint8_t[]> mem_ = std::make_unique<std::uint8_t[]>(98304);
    std::size_t used_size_ = 0;

  public:
    std::uint8_t  read_8bit(std::uint32_t addr);
    void write_8bit(std::uint32_t addr, std::uint8_t value);
    //potential increment or decrement 
    void print_memory() const;
}


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


  private:
    Flash flash_;
    SRAM sram_;


};
  
