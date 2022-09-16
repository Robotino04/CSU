#include "PositioningPass.hpp"
#include "EvaluationPass.hpp"

#include <algorithm>

PositioningPass::PositioningPass(std::string filename){
    this->filename = filename;
}


uint64_t PositioningPass::getBinarySize(){
    return maxPos;
}

Token& PositioningPass::getToken(){
    return tokens->at(index);
}
Token const& PositioningPass::getToken() const{
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
    if (std::find(types.begin(), types.end(), getToken().type) != types.end()){
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
bool PositioningPass::match(std::vector<TokenType> types) const{
    return std::find(types.begin(), types.end(), getToken().type) != types.end();
}
std::vector<Token> PositioningPass::consumeExpression(bool shouldBeAddress){
    bool hasAddress = false;
    std::vector<Token> expr;
    do{
        expr.push_back(consumeTypes({
            TokenType::Number,
            TokenType::Address,
            TokenType::Label,
            TokenType::Plus,
            TokenType::Minus,
            TokenType::Star,
            TokenType::Slash,
            TokenType::Percent,
            TokenType::Bang,
            TokenType::Tilde,
            TokenType::OpenParen,
            TokenType::CloseParen,
        }));
        hasAddress |= (expr.back().type == TokenType::Address || expr.back().type == TokenType::Label);
    }
    while (match({
        TokenType::Number,
        TokenType::Address,
        TokenType::Label,
        TokenType::Plus,
        TokenType::Minus,
        TokenType::Star,
        TokenType::Slash,
        TokenType::Percent,
        TokenType::Bang,
        TokenType::Tilde,
        TokenType::OpenParen,
        TokenType::CloseParen,
    }));
    if (shouldBeAddress && !hasAddress){
        printError(getToken().sourceInfo, "Expected expression resulting in an address");
    }
    else if (!shouldBeAddress && hasAddress){
        printError(getToken().sourceInfo, "Expected expression resulting in a number");
    }
    return expr;
}

std::vector<Token>& PositioningPass::operator() (std::vector<Token>& tokens){
    this->tokens = &tokens;

    pushErrorContext("positioning tokens in " + filename);

    while (index < tokens.size()){
        if (getToken().type == TokenType::Instruction){
            pushErrorContext("positioning instruction " + getToken().lexeme);
            auto& instr = consumeType(TokenType::Instruction);
            if (instr.lexeme == "subleq"){
                consumeExpression(true);
                pos+=2;
                consumeType(TokenType::Comma);
                consumeExpression(true);
                pos+=2;
                consumeType(TokenType::Comma);
                consumeExpression(true);
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
                consumeExpression(false);
                pos++;
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
            }
            else if (kwd.lexeme == ".address"){
                consumeExpression(true);
                pos+=2;
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
            }
            else if (kwd.lexeme == ".org"){
                auto tmp = consumeExpression(true);
                // construct a parsable token sequence
                std::vector<Token> expr;
                expr.reserve(tmp.size()+2);

                expr.push_back(Token(TokenType::BeginOfFile, ""));
                expr.insert(expr.end(), tmp.begin(), tmp.end());
                expr.push_back(Token(TokenType::EndOfFile, '\0'));

                // evaluate the expression
                EvaluationPass evaluater("evaluating .org expression");
                expr = evaluater(expr);

                // validate the result
                if (expr.size() && expr.at(1).type == TokenType::Address){
                    pos = std::any_cast<uint16_t>(expr.at(1).data);
                }
                else{
                    printError(kwd.sourceInfo, ".org directive not followed by address");
                }
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
            }
            else if (kwd.lexeme == ".asciiz"){
                auto str = consumeType(TokenType::String);
                pos += std::any_cast<std::string>(str.data).size() + 1; // The null termination
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
        maxPos = std::max(maxPos, pos);
    }

    return tokens;
}