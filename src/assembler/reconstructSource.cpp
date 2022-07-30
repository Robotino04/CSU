#include "parsingFunctions.hpp"


std::string reconstructSource(std::vector<Token> const& tokens){
    std::string source = "; Reconstructed source code\n";
    for (auto const& tok : tokens){
        if (tok.type == TokenType::Instruction){
            source += tok.lexeme;
            source += " ";
        }
        else if (tok.type == TokenType::Keyword){
            source += tok.lexeme;
            source += " ";
        }
        else if (tok.type == TokenType::Comma){
            source += tok.lexeme;
            source += " ";
        }
        else if (tok.type == TokenType::Newline){
            source += "\n";
        }
        else if (tok.type == TokenType::EndOfFile){}
        else{
            source += tok.lexeme;
        }
    }
    return source + "\n";
}