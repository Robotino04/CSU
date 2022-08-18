#include "SourceReconstructionPass.hpp"

SourceReconstructionPass::SourceReconstructionPass(std::string filename){
    this->filename = filename;
}

std::string SourceReconstructionPass::getSource(){
    return source;
}

std::vector<Token>& SourceReconstructionPass::operator() (std::vector<Token>& tokens){
    source = "; Reconstructed source code\n";

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
    source += "\n";
    return tokens;
}