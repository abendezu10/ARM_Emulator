#include "memory.hpp"
#include "bin_file.hpp"

using namespace std;

vector<uint8_t> init_memory(BinFile& binfile){
  // creates memory by using a vector and copying each byte from file and using .pushback()
  vector<uint8_t> memory;
  uintmax_t size = binfile.get_file_size();
  uint8_t current_byte;

  while(size){
    current_byte = binfile.get_byte();
    memory.push_back(current_byte);

    size--;
  }

  return memory;
}

void print_memory(vector<uint8_t>& memory){
  
  vector<uint8_t>::iterator start = memory.begin();
  uintmax_t size = memory.size();

  cout << "The bytes are:\n";

  for(int i = 0; i < size; i++){
    cout << *(start + i) ;
  }
  
}
