#pragma once

#include <string>
#include <any>

#include "SourceInformation.hpp"

enum class TokenType{
    None,       // not used, marks an empty token
    Label,      // anything that is a c/c++ identifier
    Number,     // anything parsable by std::stoi and binary numbers like 0b010011
    Comment,    // ; <some text>
    Colon,      // :
    Comma,      // ,
    Address,    // number prefixed by $ apeccifiyng a memory location
    Keyword,    // assembler directives (.data, etc.)
    Instruction,// mnemonics
    Newline,    // \n
    EndOfFile,  // \0
};

struct Token{
    Token(): type(TokenType::None), lexeme(""){}
    Token(TokenType type, std::string const& lexeme): type(type), lexeme(lexeme){}
    Token(TokenType type, char lexeme): type(type){
        this->lexeme = lexeme;
    }
    TokenType type;
    std::string lexeme;
    std::any data;

    SourceInformation sourceInfo;
    uint64_t binaryPos = 0;
    uint64_t binarySize = 0;
};

namespace std{
    #define TO_STRING(CODE) #CODE
    #define CASE(TYPE) case TokenType::TYPE: return TO_STRING(TokenType::TYPE); break

    inline std::string to_string(TokenType const& type){
        switch (type){
            CASE(None);
            CASE(Label);
            CASE(Number);
            CASE(Comment);
            CASE(Colon);
            CASE(Keyword);
            CASE(Newline);
            CASE(Instruction);
            CASE(Comma);
            CASE(Address);
            CASE(EndOfFile);
            default:
                return "Unimplemented token nr. " + std::to_string(static_cast<int>(type));
        }
    }
    #undef TO_STRING
    #undef CASE
    
    inline std::string to_string(Token const& tok){
        return std::to_string(tok.type) + " Size = " + std::to_string(tok.binarySize) + " Position = " + std::to_string(tok.binaryPos) + " (\"" + tok.lexeme + "\")";
    }
}
