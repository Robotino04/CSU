#pragma once

#include <vector>
#include <stdint.h>

class CSU{
    public:
        CSU(int memorySize);

        void step();
        void run();

        void printState(uint64_t size = 16);

        void setMemoryRegion(uint64_t start, std::vector<uint64_t> newValues);
    
    private:
        std::vector<int64_t> memory;
        uint64_t pc = 0;
        uint64_t lastPc = 0;
};