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
    argc = 2;
    const char* argv0 = argv[0];
    argv = new const char*[2];
    argv[0] = argv0;
    argv[1] = "test.s";

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
    std::stringstream buffer;
    buffer << infile.rdbuf();
    std::string source = buffer.str();
    auto tokens = tokenise(source, inFilename);

    std::cout << "-------------| Tokens |-------------\n";
    for (auto const& tok : tokens){
        std::cout << std::to_string(tok) << "\n";
    }
    std::cout << "------| Reconstructed Source |------\n";
    std::cout << reconstructSource(tokens);
    return 0;
}