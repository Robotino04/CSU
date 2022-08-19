#pragma once

#include <stdint.h>
#include <vector>
#include <ostream>

// from https://gist.github.com/shreyasbharath/32a8092666303a916e24a81b18af146b
// modified to have a start parameter
void HexDump(const std::vector<uint8_t>& bytes, std::ostream& stream, size_t start = 0);


// converts two bytes (little endian) to a 16 bit integer
uint16_t toAddress(uint8_t a, uint8_t b);