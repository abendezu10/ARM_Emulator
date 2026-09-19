#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

#include "bin_file.hpp"
#include "memory.hpp"

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
  if(argv[1] == NULL){
    cout << "Missing argument!";
    return -1;
  }

  BinFile binfile(argv[1]);  

  // Initialize memory 
  //
  Memory memory(binfile);

  cout << "The first byte is: " << static_cast<int>(memory.read_8bit(0)) << "\n";

  memory.write_8bit(0, static_cast<uint8_t>(6));

  cout << "The first byte is: " << static_cast<int>(memory.read_8bit(0x0)) << "\n";

  memory.print_memory();


  return 0;
}
