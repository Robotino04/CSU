#include "PositioningPass.hpp"

#include <algorithm>

PositioningPass::PositioningPass(std::string filename){
    this->filename = filename;
}


uint64_t PositioningPass::getBinarySize(){
    return pos;
}

Token& PositioningPass::getToken(){
    return tokens->at(index);
}

Token& PositioningPass::consumeType(TokenType type){
    if (getToken().type == type){
        tokens->at(index).binaryPos = pos;
        return tokens->at(index++);
    }
    else{
        printError(getToken().sourceInfo, "Expected " + std::to_string(type) + ", but got " + std::to_string(getToken()) + "!");
        return tokens->back();
    }
}
Token& PositioningPass::consumeTypes(std::vector<TokenType> types){
    if (std::find(types.begin(), types.end(), getToken().type)  != types.end()){
        tokens->at(index).binaryPos = pos;
        return tokens->at(index++);
    }
    else{
        std::stringstream ss;
        ss << "Expected ";
        for (int i=0; i<types.size(); i++){
            ss << std::to_string(types.at(i))<< ",";
        }
        ss << "but got " << std::to_string(getToken()) + "!\n";
        printError(getToken().sourceInfo, ss.str());
        return tokens->back();
    }
}

std::vector<Token>& PositioningPass::operator() (std::vector<Token>& tokens){
    this->tokens = &tokens;

    pushErrorContext("positioning tokens in " + filename);

    while (index < tokens.size()){
        if (getToken().type == TokenType::Instruction){
            pushErrorContext("positioning instruction " + getToken().lexeme);
            auto& instr = consumeType(TokenType::Instruction);
            if (instr.lexeme == "subleq"){
                consumeTypes({TokenType::Label, TokenType::Address}).binarySize = sizeof(uint16_t);
                pos+=2;
                consumeType(TokenType::Comma);
                consumeTypes({TokenType::Label, TokenType::Address}).binarySize = sizeof(uint16_t);
                pos+=2;
                consumeType(TokenType::Comma);
                consumeTypes({TokenType::Label, TokenType::Address}).binarySize = sizeof(uint16_t);
                pos+=2;
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
            }
            else{
                printError(instr.sourceInfo, "Unimplemented instruction \"" + instr.lexeme + "\"!");
            }
            popErrorContext();
        }
        else if (getToken().type == TokenType::Keyword){
            pushErrorContext("positioning keyword " + getToken().lexeme);
            auto& kwd = consumeType(TokenType::Keyword);
            if (kwd.lexeme == ".data"){
                consumeType(TokenType::Number).binarySize = sizeof(int8_t);
                pos++;
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
            }
            else{
                printError(kwd.sourceInfo, "Unimplemented keyword \"" + kwd.lexeme + "\"!");
            }
            popErrorContext();
        }
        else{
            consumeType(getToken().type);
        }
    }

    return tokens;
}