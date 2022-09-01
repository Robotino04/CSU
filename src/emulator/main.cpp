#include <iostream>
#include <math.h>
#include <fstream>

#include "CSU.hpp"

void printHelp(const char* argv0){
    std::cout << "Usage: " << argv0 << " [options] file\n";
    std::cout << " -h/--help\tPrint this help text.\n";
}

int main(int argc, const char** argv) {
    #ifndef NDEBUG
    // hardcode the arguments for easier debuging
    argc = 2;
    const char* argv0 = argv[0];
    argv = new const char*[2];
    argv[0] = argv0;
    argv[1] = "a.out";
    #endif

    if (argc < 2){
        std::cout << "Invalid number of arguments!\n";
        return 1;
    }

    for (int i=1;i<argc-1;i++){
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help"){
            printHelp(argv[0]);
            return 0;
        }
        else{
            std::cout << "Unknown argument \"" << arg << "\"!\n";
            return 1;
        }
    }

    // read in the binary
    std::string inFilename = argv[argc-1];
    std::ifstream infile(inFilename, std::ios::binary);
    if (!infile.is_open()){
        std::cout << "Error opening input file!\n";
        return 1;
    }
    std::vector<int8_t> binary;
    infile.seekg(0, std::ios::end);
    int length = infile.tellg();
    infile.seekg(0, std::ios::beg);
    binary.resize(length);
    infile.read(reinterpret_cast<char*>(binary.data()), length);
    infile.close();





    CSU csu(binary.size());

    csu.setMemoryRegion(0, binary);

    csu.run();
    csu.printState(128);


    return 0;
}