#include "CSU.hpp"

#include <memory.h>
#include <iostream>

CSU::CSU(int memorySize){
    memory.resize(memorySize);
}

void CSU::step(){
    lastPc = pc;
    const uint64_t a_ptr = memory[pc++];
    const uint64_t b_ptr = memory[pc++];
    const uint64_t c_ptr = memory[pc++];

    memory[b_ptr] -= memory[a_ptr];
    if (memory[b_ptr] <= 0)
        pc = c_ptr;
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
        
    std::cout << "\nMemory from 0x" << start << " to 0x" << end << ":\n";
    for (uint64_t i = start; i < end; i++){
        if ((i-start) % 8 == 0)
            std::cout << "\n\t";
        std::cout << memory.at(i) << "\t";
    }
    std::cout << "\n" << std::dec;
}


void CSU::setMemoryRegion(uint64_t start, std::vector<int64_t> newValues){
    if (start + newValues.size() > memory.size())
        throw std::out_of_range("Invalid memory region");

    std::copy(newValues.begin(), newValues.end(), memory.begin() + start); 
}