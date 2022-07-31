#include "parsingFunctions.hpp"

#include <algorithm>

void resolveLabels(std::vector<Token>& tokens){
    for (int i=0;i<tokens.size(); i++){
        if (tokens.at(i).type == TokenType::Label){
            if (i+1 < tokens.size() && tokens.at(i+1).type == TokenType::Colon) continue;

            std::vector<Token>::iterator targetLabel = tokens.begin()-1;
            do {
                targetLabel = std::find(targetLabel+1, tokens.end(), tokens.at(i));
            }
            while ((targetLabel+1)->type != TokenType::Colon);

            tokens.at(i).type = TokenType::Address;
            tokens.at(i).lexeme = "$" + std::to_string(targetLabel->binaryPos);
            tokens.at(i).data = (uint64_t)targetLabel->binaryPos;
        }
    }
}