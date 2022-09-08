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
    binary.resize(binarySize);

    uint16_t binaryPos = 0;
    size_t index;
    try{
        for (index = 0; index < tokens.size(); index++){
            auto const& tok = tokens.at(index);
            if (tok.type == TokenType::Address){
                auto addr = std::any_cast<uint16_t>(tok.data);

                // push the address in little endian
                binary.at(binaryPos++) = addr & 0xFF;
                binary.at(binaryPos++) = addr >> 8;
            }
            else if (tok.type == TokenType::Number){
                binary.at(binaryPos++) = std::any_cast<int8_t>(tok.data);
            }
            else if (tok.type == TokenType::Keyword){
                if (tok.lexeme == ".org"){
                    if (index+1 >= tokens.size()) printError(tok.sourceInfo, ".org directive not followed by address (Hit EOF while parsing)");
                    auto const& address = tokens.at(++index);
                    if (address.type != TokenType::Address) printError(tok.sourceInfo, ".org directive not followed by address");
                    
                    binaryPos = std::any_cast<uint16_t>(address.data);
                }
            }
        }
    }
    catch(std::out_of_range){
        printError(tokens.at(index).sourceInfo, "Estamated binary size exceeded!");
    }

    return tokens;
}