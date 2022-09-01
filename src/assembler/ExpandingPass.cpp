#include "ExpandingPass.hpp"

#include <algorithm>
#include <iostream>

ExpandingPass::ExpandingPass(std::string filename){
    this->filename = filename;
}


bool ExpandingPass::isDone() const{
    return tokensIt == tokens->end();
}
Token ExpandingPass::getToken() const{
    return *tokensIt;
}
Token ExpandingPass::getNextToken() const{
    if (std::next(tokensIt) == tokens->end()){
        return tokens->back();
    }
    return *std::next(tokensIt);
}
bool ExpandingPass::matchNext(TokenType type) const{
    return getNextToken().type == type;
}
bool ExpandingPass::match(TokenType type) const{
    return getToken().type == type;
}
bool ExpandingPass::match(std::vector<TokenType> types) const{
    return std::find(types.begin(), types.end(), getToken().type) != types.end();
}
Token ExpandingPass::consume(){
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
Token ExpandingPass::consumeType(TokenType type){
    if (getToken().type == type){
        return consume();
    }
    else{
        printError(getToken().sourceInfo, "Expected " + std::to_string(type) + " but got " + std::to_string(getToken()));
        return tokens->back();
    }
}
Token ExpandingPass::consumeTypes(std::vector<TokenType> types){
    if (std::find(types.begin(), types.end(), getToken().type) != types.end()){
        return consume();
    }
    else{
        std::stringstream ss;
        ss << "Expected ";
        for (int i=0; i<types.size(); i++){
            ss << std::to_string(types.at(i)) << (i==types.size()-1 ? "" : ", ");
        }
        ss << " but got " << std::to_string(getToken());
        printError(getToken().sourceInfo, ss.str());
        return tokens->back();
    }
}
void ExpandingPass::addGeneratedToken(TokenType type, std::string const& lexeme){
    tokens->insert(tokensIt, Token(type, lexeme))->sourceInfo = std::prev(tokensIt)->sourceInfo;
}
std::vector<Token> ExpandingPass::consumeExpressionTypes(){
    std::vector<Token> tokens;
    do{
        tokens.push_back(consumeTypes({
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
    }while (match({
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
    return tokens;
}

std::vector<Token>& ExpandingPass::operator() (std::vector<Token>& tokens){
    // save the caller's tokens
    auto savedTokens = this->tokens;
    auto savedTokensIt = tokensIt;

    // copy the tokens into a std::list for easier manipulation
    std::list<Token> tokensList(tokens.begin(), tokens.end());
    this->tokens = &tokensList;
    this->tokensIt = this->tokens->begin();
    this->removeTokens = false;

    

    if (currentMacroName.empty()){
        // a new file is being processed
        pushErrorContext("expanding file \"" + filename + "\"");
    }
    else
        pushErrorContext("expanding macro \"" + currentMacroName + "\"");

    while (!isDone()){
        if (match(TokenType::Instruction)){
            pushErrorContext("parsing instruction");
            if (getToken().lexeme == "subleq"){
                consume();
                consumeExpressionTypes();
                consumeType(TokenType::Comma);
                consumeExpressionTypes();
                
                if (match(TokenType::Comma)){
                    // the jump address is given
                    consumeType(TokenType::Comma);
                    consumeExpressionTypes();
                    consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                }
                else{
                    // insert the address of the next instruction to ignore the jump
                    std::string nextInstruction = generateUniqueLabel(currentMacroName);
                    addGeneratedToken(TokenType::Comma, ",");
                    addGeneratedToken(TokenType::Label, nextInstruction);
                    addGeneratedToken(TokenType::Newline, "\n");
                    addGeneratedToken(TokenType::Label, nextInstruction);
                    addGeneratedToken(TokenType::Colon, ":");
                    consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                }
            }
            else{
                printError(getToken().sourceInfo, "Unknown instruction " + getToken().lexeme + "!");
            }
            popErrorContext();
        }
        else if (match(TokenType::Label)){
            // check if the label is a macro
            auto macro = std::find_if(macros.begin(), macros.end(), [&](Macro const& m){
                return m.name == getToken().lexeme;
            });

            if (macro != macros.end()){
                // it is one
                removeTokens = true;
                consumeType(TokenType::Label);
                // expand the macro
                std::list<Token> bodyCopy(macro->body.begin(), macro->body.end());
                
                pushErrorContext("expanding macro arguments for \"" + macro->name + "\"");
                // replace the macro arguments with values from the macro call
                for (int i=0; i<macro->arguments.size(); i++){
                    auto value = consumeExpressionTypes();
                    auto toReplace = std::find_if(bodyCopy.begin(), bodyCopy.end(), [&](Token const& t){
                        return t.type == TokenType::Label && t.lexeme == macro->arguments.at(i);
                    });
                    
                    while (toReplace != bodyCopy.end()){
                        toReplace = bodyCopy.erase(toReplace);
                        bodyCopy.insert(toReplace, value.begin(), value.end());
                        std::advance(toReplace, value.size());


                        toReplace = std::find_if(toReplace, bodyCopy.end(), [&](Token const& t){
                            return t.type == TokenType::Label && t.lexeme == macro->arguments.at(i);
                        });
                    }

                    if (i != macro->arguments.size()-1){
                        if (match(TokenType::Comma)){
                            consumeType(TokenType::Comma);
                        }
                        else{
                            printError(getToken().sourceInfo, "While expanding macro " + macro->name + ": Expected a macro argument, but got " + std::to_string(getToken()) + "!");
                        }
                    }
                }
                popErrorContext();
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                std::vector<Token> expandedBody;
                {
                    std::string savedMacroName = currentMacroName;
                    currentMacroName = macro->name;
                    std::vector<Token> bodyCopyVector(bodyCopy.begin(), bodyCopy.end());
                    expandedBody = (*this)(bodyCopyVector);
                    currentMacroName = savedMacroName;
                }

                // uniquify the local labels
                pushErrorContext("uniquifying local labels");
                for (int i=0; i<macro->localLabels.size(); i++){
                    std::replace_if(expandedBody.begin(), expandedBody.end(), [&](Token const& t){
                        return t.type == TokenType::Label && t.lexeme == macro->localLabels.at(i);
                    }, Token(TokenType::Label, uniqueifyLabel(macro->localLabels.at(i), macro->name)));
                }
                if (!macro->localLabels.empty()){
                    auto x = Token(TokenType::Label, uniqueifyLabel(macro->localLabels.at(0), macro->name));
                    std::to_string(x);
                }
                popErrorContext();
                this->tokens->insert(tokensIt, expandedBody.begin(), expandedBody.end());
                removeTokens = false;
            }
            else{
                // just add the label
                consume();
            }
        }
        
        else if (match(TokenType::Keyword)){
            if (getToken().lexeme == ".macro"){
                pushErrorContext("parsing macro definition");
                removeTokens = true;
                consumeType(TokenType::Keyword);
                Macro macro;
                macro.name = consumeType(TokenType::Label).lexeme;
                pushErrorContext("parsing macro arguments for \"" + macro.name + "\"");
                if (match(TokenType::Label)){
                    // parse the arguments separated by commas
                    macro.arguments.push_back(consumeType(TokenType::Label).lexeme);
                    while (match(TokenType::Comma)){
                        consumeType(TokenType::Comma);
                        macro.arguments.push_back(consumeType(TokenType::Label).lexeme);
                    }
                }
                popErrorContext();

                // parse the macro body
                pushErrorContext("parsing macro body for \"" + macro.name + "\"");
                consumeType(TokenType::OpenBrace);
                while (!match(TokenType::CloseBrace)){
                    if (match(TokenType::Label) && matchNext(TokenType::Colon)){
                        macro.localLabels.push_back(getToken().lexeme);
                    }
                    macro.body.push_back(consume());
                }
                consumeType(TokenType::CloseBrace);
                popErrorContext();
                removeTokens = false;
                macros.push_back(macro);
                popErrorContext();
            }
            else if (getToken().lexeme == ".data"){
                pushErrorContext("parsing \".data\" directive");
                consumeType(TokenType::Keyword);
                consumeExpressionTypes();
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                popErrorContext();
            }
            else if (getToken().lexeme == ".address"){
                pushErrorContext("parsing \".address\" directive");
                consumeType(TokenType::Keyword);
                consumeExpressionTypes();
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                popErrorContext();
            }
            else{
                printError(getToken().sourceInfo, "Unknown keyword " + getToken().lexeme + "!");
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