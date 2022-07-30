#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "Token.hpp"
#include "SourceInformation.hpp"

inline void printError(SourceInformation info, std::string msg)
{
    std::cout << info.filename << ":" << info.line << ":" << info.column << ": " << msg;
}

inline std::string generateUniqueLabel()
{
    static uint64_t counter;
    std::stringstream ss;
    ss << "label_" << std::hex << counter++ << std::dec;
    return ss.str();
}

std::vector<Token> tokenise(std::string source, std::string filename);

std::string reconstructSource(std::vector<Token> const &tokens);