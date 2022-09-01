#include "SourceReconstructionPass.hpp"

#include <iomanip>

SourceReconstructionPass::SourceReconstructionPass(std::string filename){
    this->filename = filename;
}

std::string SourceReconstructionPass::getSource(){
    return source.str();
}

std::vector<Token>& SourceReconstructionPass::operator() (std::vector<Token>& tokens){
    source.clear();
    source << "; Reconstructed source code\n";

    for (auto const& tok : tokens){
        if (tok.type == TokenType::Instruction){
            source << tok.lexeme << " ";
        }
        else if (tok.type == TokenType::Address){
            source << "0x" << std::setw(4) << std::setfill('0') << std::hex << std::any_cast<uint16_t>(tok.data) << std::dec;
        }
        else if (tok.type == TokenType::Keyword){
            source << tok.lexeme << " ";
        }
        else if (tok.type == TokenType::Comma){
            source << tok.lexeme << " ";
        }
        else if (tok.type == TokenType::Newline){
            source << "\n";
        }
        else if (tok.type == TokenType::EndOfFile){}
        else{
            source << tok.lexeme;
        }
    }
    source << "\n";
    return tokens;
}