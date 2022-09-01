#include "CSU.hpp"
#include "utils.hpp"

#include <memory.h>
#include <iostream>
#include <array>

CSU::CSU(int memorySize){
    memory.resize(memorySize);
}

void CSU::step(){
    lastPc = pc;
    std::array<uint16_t, 3> instruction;
    for (int i=0;i<3;i++){
        auto a = memory[pc++];
        auto b = memory[pc++];
        instruction[i] = toAddress(a, b);
    }
    const auto a_ptr = instruction[0];
    const auto b_ptr = instruction[1];
    const auto c = instruction[2];

    memory[b_ptr] -= memory[a_ptr];
    if (memory[b_ptr] <= 0)
        pc = c;
}
void CSU::run(){
    while (pc < memory.size()){
        lastPc = pc;
        step();
    }
}

void CSU::printState(uint64_t size){
    std::cout << std::hex << "CSU (size=0x" << memory.size() << ") last at 0x" << lastPc << " (exit at 0x" << pc << "):";

    // print the memory region around the last program counter
    uint64_t start = lastPc - size/2;
    uint64_t end = lastPc + size/2;
    if (start > memory.size())
        start = 0;
    if (end > memory.size())
        end = memory.size();
    
    // round down to multiple of 16 (cut the last 4 bits)
    start = start & ~0xF;
        
    std::cout << "\nMemory from 0x" << start << " to 0x" << end-1 << ":\n";
    HexDump(std::vector<uint8_t>(memory.begin(), memory.begin() + end), std::cout, start);
}


void CSU::setMemoryRegion(uint64_t start, std::vector<int8_t> newValues){
    if (start + newValues.size() > memory.size())
        throw std::out_of_range("Invalid memory region");

    std::copy(newValues.begin(), newValues.end(), memory.begin() + start); 
}