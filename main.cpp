#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <iomanip>

#include "bin_file.hpp"
#include "memory.hpp"
#include "cpu.hpp"

/*
 *Going to need to add the stack pointer and reset vector with thumb bit and the start of the program
 * 0x0 Initial SP
 * 0x4 Reset Vector 0x00 00 00 09(1001) bit 0 = 1 so it is Thumb state
 * 0x8 Start of program 0x2005 - MOVS R0, #5 
 */

using namespace std;

int main(int argc, char *argv[] ){

  cout << "Starting program..." << endl;

  /*
   * 1. need to import the BIN file into the program
   * 2. create virtual memory space to store bin file
   *  a.) Option 1: get the size of the file in bytes and allocate an array that will store all the bytes
   *  b.) Option 2: read in 32 (or more due to thumb instructiosn) bit intervals and using a fixed 32 bit buffer, to store the 32 bits and to read off it
   *  c.) Option 3: use vector data type for memory
   *
   *  2a. I used a vector object to create virtual memory of the emulator but what about
   *  perhaps I make a class 
   */
  if(argv[1] == nullptr){
    cout << "Missing argument!";
    return -1;
}

  BinFile binfile(argv[1]);  

  // Initialize memory 
  Memory memory(binfile);

  // Initialize cpu
  Cpu cpu;

  while(cpu.read_reg(Cpu::Register::PC) < static_cast<uint32_t>(binfile.get_file_size())){
    cpu.cycle(memory);
  }
  //cout << "PC = 0x" << hex << setw(8) << setfill('0') << memory.read_16bit(0) << '\n';
  return 0;
}
