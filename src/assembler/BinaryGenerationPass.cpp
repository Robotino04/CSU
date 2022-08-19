#include "BinaryGenerationPass.hpp"

BinaryGenerationPass::BinaryGenerationPass(std::string filename, uint64_t binarySize){
    this->filename = filename;
    this->binarySize = binarySize;
}

std::vector<int8_t> const& BinaryGenerationPass::getBinary(){
    return binary;
}

std::vector<Token>& BinaryGenerationPass::operator() (std::vector<Token>& tokens){
    binary.clear();
    binary.reserve(binarySize);

    for (auto const& tok : tokens){
        if (tok.type == TokenType::Address){
            auto addr = std::any_cast<uint16_t>(tok.data);

            // push the address in little endian
            binary.push_back(addr & 0xFF);
            binary.push_back(addr >> 8);
        }
        else if (tok.type == TokenType::Number){
            binary.push_back(std::any_cast<int8_t>(tok.data));
        }
    }

    return tokens;
}