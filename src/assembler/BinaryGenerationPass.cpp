#include "BinaryGenerationPass.hpp"

BinaryGenerationPass::BinaryGenerationPass(std::string filename, uint64_t binarySize){
    this->filename = filename;
    this->binarySize = binarySize;
}

std::vector<int64_t> const& BinaryGenerationPass::getBinary(){
    return binary;
}

std::vector<Token>& BinaryGenerationPass::operator() (std::vector<Token>& tokens){
    binary.clear();
    binary.reserve(binarySize / sizeof(uint64_t));

    for (auto const& tok : tokens){
        if (tok.type == TokenType::Address){
            binary.push_back(std::any_cast<uint64_t>(tok.data));
        }
        else if (tok.type == TokenType::Number){
            binary.push_back(std::any_cast<int64_t>(tok.data));
        }
    }

    return tokens;
}