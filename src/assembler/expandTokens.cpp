#include "parsingFunctions.hpp"

#include <algorithm>

std::vector<Token> expandTokens(std::vector<Token> const& tokens){
    std::vector<Token> newTokens;
    int index=0;

    auto const getToken = [&]() -> Token const&{
        return tokens.at(index);
    };
    auto const match = [&](TokenType type) -> bool{
        return getToken().type == type;
    };
    auto const consumeType = [&](TokenType type)-> Token const&{
        if (getToken().type == type){
            newTokens.push_back(tokens.at(index));
            return tokens.at(index++);
        }
        else{
            printError(getToken().sourceInfo, "Expected " + std::to_string(type) + ", but got " + std::to_string(getToken()) + "!\n");
            exit(1);
        }
    };
    auto const consumeTypes = [&](std::vector<TokenType> types)-> Token const&{
        if (std::find(types.begin(), types.end(), getToken().type)  != types.end()){
            newTokens.push_back(tokens.at(index));
            return tokens.at(index++);
        }
        else{
            std::stringstream ss;
            ss << "Expected ";
            for (int i=0; i<types.size(); i++){
                ss << std::to_string(types.at(i))<< ", ";
            }
            ss << "but got " << std::to_string(getToken()) + "!\n";
            printError(getToken().sourceInfo, ss.str());
            exit(1);
        }
    };
    auto const consume = [&]() -> Token const&{
        return consumeType(getToken().type);
    };
    auto const addGeneratedToken = [&](TokenType type, std::string const& lexeme){
        newTokens.emplace_back(type, lexeme).sourceInfo.filename = "generated code";
    };


    while (index < tokens.size()){
        if (match(TokenType::Instruction)){
            if (getToken().lexeme == "subleq"){
                consume();
                consumeTypes({TokenType::Label, TokenType::Address});
                consumeType(TokenType::Comma);
                consumeTypes({TokenType::Label, TokenType::Address});
                
                if (match(TokenType::Comma)){
                    // the jump address is given
                    consumeType(TokenType::Comma);
                    consumeTypes({TokenType::Label, TokenType::Address});
                    consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                }
                else{
                    // insert the address of the next instruction to ignore the jump
                    auto nextInstruction = generateUniqueLabel();
                    addGeneratedToken(TokenType::Comma, ",");
                    addGeneratedToken(TokenType::Label, nextInstruction);
                    addGeneratedToken(TokenType::Newline, "\n");
                    addGeneratedToken(TokenType::Label, nextInstruction);
                    addGeneratedToken(TokenType::Colon, ":");
                    consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                }
            }
        }
        else{
            consume();
        }
    }
    return newTokens;
}