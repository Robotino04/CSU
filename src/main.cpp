#include <iostream>
#include <math.h>

#include "CSU/CSU.hpp"

int main(){
    CSU csu(std::pow(2, 4));

    csu.setMemoryRegion(0,{
        9, 11, 3,
        11, 10, 6,
        11, 11, -1ULL,
        5,7,0
    });

    csu.run();
    csu.printState(32);


    return 0;
}