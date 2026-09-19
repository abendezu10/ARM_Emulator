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
   */
  if(argv[1] == NULL){
    cout << "Missing argument!";
    return -1;
  }

  BinFile binfile(argv[1]);  

  // Initialize memory 

  vector<uint8_t> memory = init_memory(binfile); 

  print_memory(memory);

  return 0;
}
