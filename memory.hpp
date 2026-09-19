#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

class BinFile;

std::vector<uint8_t> init_memory(BinFile& binfile);

void print_memory(std::vector<uint8_t>& memory);

