#include "parsingFunctions.hpp"


std::vector<int64_t> generateBinary(std::vector<Token> const& tokens, uint64_t binarySize){
    std::vector<int64_t> binary;
    binary.reserve(binarySize / sizeof(uint64_t));

    for (auto const& tok : tokens){
        if (tok.type == TokenType::Address){
            binary.push_back(std::any_cast<uint64_t>(tok.data));
        }
        else if (tok.type == TokenType::Number){
            binary.push_back(std::any_cast<int64_t>(tok.data));
        }
    }

    return binary;
}