#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "memory.hpp"

using namespace std;

class BinFile{
  public:
    explicit BinFile(const string& path){ 
      bin_fd_.open(path, ios::binary);

      if(!bin_fd_.is_open()){
        cout << "Opening file in constructor...FAILED" << endl;
        return;
      }

      file_size_ = filesystem::file_size(path);
    }

  uintmax_t get_file_size() const{
    return file_size_;
  }

  private:
    ifstream bin_fd_;
    uintmax_t file_size_;
     
};

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


  return 0;
}
