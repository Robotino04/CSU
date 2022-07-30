#include "parsingFunctions.hpp"

#include <sstream>
#include <algorithm>

uint64_t positionTokens(std::vector<Token>& tokens){
    uint64_t pos = 0;
    int index = 0;

    auto const getToken = [&]() -> Token&{
        return tokens.at(index);
    };

    auto const consumeType = [&](TokenType type)-> Token&{
        if (getToken().type == type){
            tokens.at(index).binaryPos = pos;
            return tokens.at(index++);
        }
        else{
            printError(getToken().sourceInfo, "Expected " + std::to_string(type) + ", but got " + std::to_string(getToken()) + "!\n");
            exit(1);
        }
    };
    auto const consumeTypes = [&](std::vector<TokenType> types)-> Token&{
        if (std::find(types.begin(), types.end(), getToken().type)  != types.end()){
            tokens.at(index).binaryPos = pos;
            return tokens.at(index++);
        }
        else{
            std::stringstream ss;
            ss << "Expected ";
            for (int i=0; i<types.size(); i++){
                ss << std::to_string(types.at(i))<< ",";
            }
            ss << "but got " << std::to_string(getToken()) + "!\n";
            printError(getToken().sourceInfo, ss.str());
            exit(1);
        }
    };

    while (index < tokens.size()){
        if (getToken().type == TokenType::Instruction){
            auto& instr = consumeType(TokenType::Instruction);
            if (instr.lexeme == "subleq"){
                consumeTypes({TokenType::Label, TokenType::Address}).binarySize = sizeof(uint64_t);
                pos += sizeof(uint64_t);
                consumeType(TokenType::Comma);
                consumeTypes({TokenType::Label, TokenType::Address}).binarySize = sizeof(uint64_t);
                pos += sizeof(uint64_t);
                consumeType(TokenType::Comma);
                consumeTypes({TokenType::Label, TokenType::Address}).binarySize = sizeof(uint64_t);
                pos += sizeof(uint64_t);
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
            }
            else{
                printError(instr.sourceInfo, "Unimplemented instruction \"" + instr.lexeme + "\"!\n");
            }
        }
        else if (getToken().type == TokenType::Keyword){
            auto& kwd = consumeType(TokenType::Keyword);
            if (kwd.lexeme == ".data"){
                consumeType(TokenType::Number).binarySize = sizeof(uint64_t);
                pos += sizeof(uint64_t);
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
            }
            else{
                printError(kwd.sourceInfo, "Unimplemented keyword \"" + kwd.lexeme + "\"!\n");
            }
        }
        else{
            consumeType(getToken().type);
        }
    }

    return pos;
}