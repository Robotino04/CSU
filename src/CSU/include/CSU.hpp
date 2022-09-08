#pragma once

#include <array>
#include <vector>
#include <stdint.h>

class CSU{
    public:
        void step();
        void run();

        void printState(uint64_t size = 16);

        void setMemoryRegion(uint64_t start, std::vector<uint8_t> newValues);

        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t data);
    
    private:
        std::array<uint8_t, 0x8000> memory;
        uint16_t pc = 0;
        uint16_t lastPc = 0;
};