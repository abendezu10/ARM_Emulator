#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

class BinFile{
  private:
    std::ifstream bin_fd_;
    std::uintmax_t file_size_;

  public:
    explicit BinFile(const std::string& path);
     
    std::uintmax_t get_file_size() const;
    std::uint8_t get_byte();

};
