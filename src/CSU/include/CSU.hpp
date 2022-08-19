#pragma once

#include <vector>
#include <stdint.h>

class CSU{
    public:
        CSU(int memorySize);

        void step();
        void run();

        void printState(uint64_t size = 16);

        void setMemoryRegion(uint64_t start, std::vector<int8_t> newValues);
    
    private:
        std::vector<int8_t> memory;
        uint16_t pc = 0;
        uint16_t lastPc = 0;
};