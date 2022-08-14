#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Token.hpp"
#include "parsingFunctions.hpp"


void printHelp(const char* argv0){
    std::cout << "Usage: " << argv0 << " [options] file\n";
    std::cout << " -o outfile\tWrite the binary to outfile. Default is \"a.out\"\n";
    std::cout << " -h/--help\tPrint this help text.\n";
}

int main(int argc, const char** argv){
    // hardcode the arguments for easier debuging
    #ifndef NDEBUG
    argc = 2;
    const char* argv0 = argv[0];
    argv = new const char*[2];
    argv[0] = argv0;
    argv[1] = "test.s";
    #endif

    std::string outFilename = "a.out";
    if (argc < 2){
        std::cout << "Invalid number of arguments!\n";
        return 1;
    }
    for (int i=1;i<argc-1;i++){
        std::string arg = argv[i];
        if (arg == "-o"){
            if (i+1 < argc){
                outFilename = argv[++argc];
            }
            else{
                std::cout << "No output file after \"-o\"!\n";
                return 1;
            }
        }
        else if (arg == "-h" || arg == "--help"){
            printHelp(argv[0]);
            return 0;
        }
        else{
            std::cout << "Unknown argument \"" << arg << "\"!\n";
            return 1;
        }
    }
    std::string inFilename = argv[argc-1];

    std::ifstream infile(inFilename);
    if (!infile.is_open()){
        std::cout << "Error opening input file!\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << infile.rdbuf();
    std::string source = buffer.str();

    std::vector<Macro> macros;
    auto tokens = expandTokens(tokenise(source, inFilename), macros);
    uint64_t binarySize = positionTokens(tokens);

    std::cout << "-------------| Tokens |-------------\n";
    for (auto const& tok : tokens){
        std::cout << std::to_string(tok) << "\n";
    }

    std::cout << "------| Reconstructed Source |------\n";
    std::cout << reconstructSource(tokens);
    
    resolveLabels(tokens);
    auto binary = generateBinary(tokens, binarySize);

    int binarySizeSum = 0;
    

    std::cout << "-------------| Binary |-------------\n";
    std::cout << "The binary is " << binary.size() * sizeof(int64_t) << " bytes large.\n";
    if (binary.size() * sizeof(int64_t) != binarySize){
        std::cout << "The estimated size was " << binarySize << "bytes!";
    }
    std::ofstream outfile(outFilename, std::ios::binary);
    if (!outfile.is_open()){
        std::cout << "Error opening output file!\n";
        return 1;
    }
    outfile.write(reinterpret_cast<char*>(binary.data()), sizeof(int64_t) * binary.size() / sizeof(char));

    return 0;
}