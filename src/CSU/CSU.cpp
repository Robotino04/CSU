#include "CSU.hpp"
#include "utils.hpp"

#include <memory.h>
#include <iostream>
#include <array>


void CSU::step(){
    lastPc = pc;
    std::array<uint16_t, 3> instruction;
    for (int i=0;i<3;i++){
        const auto a = read(pc++);
        const auto b = read(pc++);
        instruction[i] = toAddress(a, b);
    }
    const auto a_ptr = instruction[0];
    const auto b_ptr = instruction[1];
    const auto c = instruction[2];

    const auto a = read(a_ptr);
    const auto b = read(b_ptr);

    write(b_ptr, b - a);
    if (a >= b)
        pc = c;
}

void CSU::run(){
    while (pc < memory.size()){
        lastPc = pc;
        step();
        if (pc >= 0x8000) break;
    }
}

uint8_t CSU::read(uint16_t address){
    if (address <= 0x7FFF){
        return memory[address];
    }
    else if (address == 0x8000){
        return 0;
    }
    else if (address == 0x8001){
        return std::cin.get();
    }
    else{
        std::cout << "[Invalid read from " << address << "]";
        return 0;
    }
}
void CSU::write(uint16_t address, uint8_t data){
    if (address <= 0x7FFF){
        memory[address] = data;
    }
    else if (address == 0x8000){
        std::cout << static_cast<char>(data);
    }
    else if (address == 0x8001);
    else{
        std::cout << "[Invalid write to " << address << "]";
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


void CSU::setMemoryRegion(uint64_t start, std::vector<uint8_t> newValues){
    if (start + newValues.size() > memory.size())
        throw std::out_of_range("Invalid memory region");

    std::copy(newValues.begin(), newValues.end(), memory.begin() + start); 
}