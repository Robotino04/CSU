#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "Token.hpp"
#include "SourceInformation.hpp"

inline void printError(SourceInformation info, std::string msg){
    std::cout << info.filename << ":" << info.line << ":" << info.column << ": " << msg;
}

inline std::string uniqueifyLabel(std::string const& label, std::string const& macroName){
    static uint64_t counter = 0;
    if (!macroName.empty()){
        return macroName + "_" + label + std::to_string(counter++);
    }
    else{
        return label + std::to_string(counter++);
    }
}

inline std::string generateUniqueLabel(std::string const& macroName){
    return uniqueifyLabel("label", macroName);
}

std::vector<Token> tokenise(std::string source, std::string filename);
std::vector<Token> expandTokens(std::vector<Token> const& tokens, std::vector<Macro>& macros, std::string macroName = "");
uint64_t positionTokens(std::vector<Token>& tokens);
void resolveLabels(std::vector<Token>& tokens);
std::vector<int64_t> generateBinary(std::vector<Token> const& tokens, uint64_t binarySize=0);

std::string reconstructSource(std::vector<Token> const &tokens);