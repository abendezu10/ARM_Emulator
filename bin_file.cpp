#include "bin_file.hpp"

using namespace std;

BinFile::BinFile(const string& path){ 
  bin_fd_.open(path, ios::binary);

  if(!bin_fd_.is_open()){
    cout << "Opening file in constructor...FAILED" << endl;
    return;
  }

  file_size_ = filesystem::file_size(path);
}


uintmax_t BinFile::get_file_size() const{
  return BinFile::file_size_;
}

uint8_t BinFile::get_byte(){
  return bin_fd_.get();
}
