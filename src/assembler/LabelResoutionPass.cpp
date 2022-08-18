#include "LabelResolutionPass.hpp"

#include <algorithm>

LabelResolutionPass::LabelResolutionPass(std::string filename){
    this->filename = filename;
}


std::vector<Token>& LabelResolutionPass::operator() (std::vector<Token>& tokens){
    pushErrorContext("resolving labels in file \"" + filename + "\"");
    for (int i=0;i<tokens.size(); i++){
        if (tokens.at(i).type == TokenType::Label){
            if (i+1 < tokens.size() && tokens.at(i+1).type == TokenType::Colon) continue;

            std::vector<Token>::iterator targetLabel = tokens.begin()-1;
            do {
                targetLabel = std::find(targetLabel+1, tokens.end(), tokens.at(i));
            }
            while ((targetLabel+1)->type != TokenType::Colon && targetLabel != tokens.end());

            if (targetLabel == tokens.end()){
                printError(tokens.at(i).sourceInfo, "Label \"" + tokens.at(i).lexeme +"\" not found!");
            }

            tokens.at(i).type = TokenType::Address;
            tokens.at(i).lexeme = "$" + std::to_string(targetLabel->binaryPos);
            tokens.at(i).data = (uint64_t)targetLabel->binaryPos;
        }
    }
    popErrorContext();

    return tokens;
}