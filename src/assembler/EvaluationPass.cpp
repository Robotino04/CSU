#include "EvaluationPass.hpp"

#include <algorithm>
#include <iostream>

EvaluationPass::EvaluationPass(std::string filename){
    this->filename = filename;
}


bool EvaluationPass::isDone() const{
    return tokensIt == tokens->end();
}
Token EvaluationPass::getToken() const{
    return *tokensIt;
}
bool EvaluationPass::match(TokenType type) const{
    return getToken().type == type;
}
bool EvaluationPass::match(std::vector<TokenType> types) const{
    return std::find(types.begin(), types.end(), getToken().type) != types.end();
}
Token EvaluationPass::consume(){
    if (isDone()){
        printError(tokens->back().sourceInfo, "Unexpected end of file");
    }
    auto token = getToken();
    if (removeTokens){
        tokensIt = tokens->erase(tokensIt);
    }
    else{
        tokensIt++;
    }
    return token;
}
Token EvaluationPass::consumeType(TokenType type){
    if (getToken().type == type){
        return consume();
    }
    else{
        printError(getToken().sourceInfo, "Expected " + std::to_string(type) + " but got " + std::to_string(getToken()) + "!");
        return tokens->back();
    }
}
Token EvaluationPass::consumeTypes(std::vector<TokenType> types){
    if (std::find(types.begin(), types.end(), getToken().type) != types.end()){
        return consume();
    }
    else{
        std::stringstream ss;
        ss << "Expected ";
        for (int i=0; i<types.size(); i++){
            ss << std::to_string(types.at(i)) << (i==types.size()-1 ? "" : ", ");
        }
        ss << " but got " << std::to_string(getToken()) + "!\n";
        printError(getToken().sourceInfo, ss.str());
        return tokens->back();
    }
}
void EvaluationPass::addGeneratedToken(Token const& token){
    tokens->insert(tokensIt, token)->sourceInfo = std::prev(tokensIt)->sourceInfo;
}

std::vector<Token>& EvaluationPass::operator() (std::vector<Token>& tokens){
    // save the caller's tokens
    auto savedTokens = this->tokens;
    auto savedTokensIt = tokensIt;

    // copy the tokens into a std::list for easier manipulation
    std::list<Token> tokensList(tokens.begin(), tokens.end());
    this->tokens = &tokensList;
    this->tokensIt = this->tokens->begin();
    this->removeTokens = false;

    

    pushErrorContext("evaluating file \"" + filename + "\"");

    while (!isDone()){
        if (match({
            TokenType::Number,
            TokenType::Address,
            TokenType::Plus,
            TokenType::Minus,
            TokenType::Star,
            TokenType::Slash,
            TokenType::Percent,
            TokenType::Bang,
            TokenType::Tilde,
            TokenType::OpenParen,
            TokenType::CloseParen,
        })){
            removeTokens = true;
            addGeneratedToken(parseExpression());
            removeTokens = false;

            // convert to the correct types 
            if ((*std::prev(tokensIt, 2)).type == TokenType::Keyword){
                if ((*std::prev(tokensIt, 2)).lexeme == ".data"){
                    if ((*std::prev(tokensIt)).type == TokenType::Address){
                        (*std::prev(tokensIt)).type = TokenType::Number;
                        (*std::prev(tokensIt)).data = static_cast<int8_t>(
                                std::any_cast<uint16_t>(
                                    (*std::prev(tokensIt)).data
                                ));
                    }
                }
                else if ((*std::prev(tokensIt, 2)).lexeme == ".address"){
                    if ((*std::prev(tokensIt)).type == TokenType::Number){
                        (*std::prev(tokensIt)).type = TokenType::Address;
                        (*std::prev(tokensIt)).data = static_cast<uint16_t>(
                                std::any_cast<int8_t>(
                                    (*std::prev(tokensIt)).data
                                ));
                    }
                }
                else if ((*std::prev(tokensIt, 2)).lexeme == ".org"){
                    if ((*std::prev(tokensIt)).type == TokenType::Number){
                        (*std::prev(tokensIt)).type = TokenType::Address;
                        (*std::prev(tokensIt)).data = static_cast<uint16_t>(
                                std::any_cast<int8_t>(
                                    (*std::prev(tokensIt)).data
                                ));
                    }
                }
            }
        }
        else{
            consume();
        }
    }
    
    // copy the new tokens into the original vector
    tokens.clear();
    tokens.insert(tokens.end(), this->tokens->begin(), this->tokens->end());
    popErrorContext();

    // restore the caller's tokens
    this->tokens = savedTokens;
    this->tokensIt = savedTokensIt;
    return tokens;
}


Token EvaluationPass::parseExpression(){
    pushErrorContext("parsing expression");
    const auto x = parseAdditiveExp();
    popErrorContext();
    return x;
}

Token EvaluationPass::parseAdditiveExp() {
    pushErrorContext("parsing additive expression");
    auto term = parseTerm();

    while (match({TokenType::Plus, TokenType::Minus})) {
        Token operatorToken = consumeTypes({TokenType::Plus, TokenType::Minus});
        auto next_term = parseTerm();

        uint16_t a, b;
        bool isAddress = false;
        if (term.type == TokenType::Address){
            a = std::any_cast<uint16_t>(term.data);
            isAddress = true;
        }
        else{
            a = std::any_cast<int8_t>(term.data);
        }
        if (next_term.type == TokenType::Address){
            b = std::any_cast<uint16_t>(next_term.data);
            isAddress = true;
        }
        else{
            b = std::any_cast<int8_t>(next_term.data);
        }

        if (operatorToken.type == TokenType::Plus){
            if (isAddress){
                term.data = (uint16_t)(a + b);
                term.type = TokenType::Address;
            }
            else{
                term.data = (int8_t)(a + b);
                term.type = TokenType::Number;
            }
        }
        else if (operatorToken.type == TokenType::Minus){
            if (isAddress){
                term.data = (uint16_t)(a - b);
                term.type = TokenType::Address;
            }
            else{
                term.data = (int8_t)(a - b);
                term.type = TokenType::Number;
            }
        }
    }
    popErrorContext();
    return term;
}
Token EvaluationPass::parseTerm() {
    pushErrorContext("parsing term");
    auto factor = parseFactor();

    while (match({TokenType::Star, TokenType::Slash, TokenType::Percent})) {
        Token operatorToken = consumeTypes({TokenType::Star, TokenType::Slash, TokenType::Percent});
        auto next_factor = parseFactor();

        uint16_t a, b;
        bool isAddress = false;
        if (factor.type == TokenType::Address){
            a = std::any_cast<uint16_t>(factor.data);
            isAddress = true;
        }
        else{
            a = std::any_cast<int8_t>(factor.data);
        }

        if (next_factor.type == TokenType::Address){
            b = std::any_cast<uint16_t>(next_factor.data);
            isAddress = true;
        }
        else{
            b = std::any_cast<int8_t>(next_factor.data);
        }

        if (operatorToken.type == TokenType::Star){
            if (isAddress){
                factor.data = (uint16_t)(a * b);
                factor.type = TokenType::Address;
            }
            else{
                factor.data = (int8_t)(a * b);
                factor.type = TokenType::Number;
            }
        }
        else if (operatorToken.type == TokenType::Slash){
            if (isAddress){
                factor.data = (uint16_t)(a / b);
                factor.type = TokenType::Address;
            }
            else{
                factor.data = (int8_t)(a / b);
                factor.type = TokenType::Number;
            }
        }
        else if (operatorToken.type == TokenType::Percent){
            if (isAddress){
                factor.data = (uint16_t)(a % b);
                factor.type = TokenType::Address;
            }
            else{
                factor.data = (int8_t)(a % b);
                factor.type = TokenType::Number;
            }
        }
    }
    popErrorContext();
    return factor;
}
Token EvaluationPass::parseFactor() {

    if (match(TokenType::OpenParen)) {
        pushErrorContext("parsing grouped expression");
        consumeType(TokenType::OpenParen);
        auto expression = parseExpression();
        consumeType(TokenType::CloseParen);
        popErrorContext();
        return expression;
    }
    else if (match({TokenType::Bang, TokenType::Minus, TokenType::Tilde})) {
        pushErrorContext("parsing unary expression");
        Token operatorToken = consumeTypes({TokenType::Bang, TokenType::Minus, TokenType::Tilde});
        auto factor = parseFactor();

        uint16_t x;
        bool isAddress = false;
        if (factor.type == TokenType::Address){
            x = std::any_cast<uint16_t>(factor.data);
            isAddress = true;
        }
        else{
            x = std::any_cast<int8_t>(factor.data);
        }

        if (operatorToken.type == TokenType::Bang){
            if (isAddress){
                factor.data = (uint16_t)(!x);
                factor.type = TokenType::Address;
            }
            else{
                factor.data = (int8_t)(!x);
                factor.type = TokenType::Number;
            }
        }
        else if (operatorToken.type == TokenType::Minus){
            if (isAddress){
                factor.data = (uint16_t)(-x);
                factor.type = TokenType::Address;
            }
            else{
                factor.data = (int8_t)(-x);
                factor.type = TokenType::Number;
            }
        }
        else if (operatorToken.type == TokenType::Tilde){
            if (isAddress){
                factor.data = (uint16_t)(~x);
                factor.type = TokenType::Address;
            }
            else{
                factor.data = (int8_t)(~x);
                factor.type = TokenType::Number;
            }
        }
        popErrorContext();
        return factor;
    }
    else if (match({TokenType::Number, TokenType::Address}))
        return consumeTypes({TokenType::Number, TokenType::Address});
    else{
        printError(getToken().sourceInfo, "Expected expression but got " + std::to_string(getToken()));
        return {};
    }
}