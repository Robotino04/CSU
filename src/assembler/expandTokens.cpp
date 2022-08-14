#include "parsingFunctions.hpp"

#include <algorithm>

std::vector<Token> expandTokens(std::vector<Token> const& tokens, std::vector<Macro>& macros, std::string macroName){
    std::vector<Token> newTokens;
    int index=0;

    bool consumeOnly = false;

    auto const getToken = [&](int offset = 0) -> Token const&{
        try{
            return tokens.at(index + offset);
        }
        catch(std::out_of_range const&){
            return tokens.at(tokens.size() - 1);
        }
    };
    auto const matchNext = [&](TokenType type) -> bool{
        return getToken(1).type == type;
    };
    auto const match = [&](TokenType type) -> bool{
        return getToken().type == type;
    };
    auto const consumeType = [&](TokenType type)-> Token const&{
        if (getToken().type == type){
            if (!consumeOnly) newTokens.push_back(tokens.at(index));
            return tokens.at(index++);
        }
        else{
            printError(getToken().sourceInfo, "Expected " + std::to_string(type) + ", but got " + std::to_string(getToken()) + "!\n");
            exit(1);
        }
    };
    auto const consumeTypes = [&](std::vector<TokenType> types)-> Token const&{
        if (std::find(types.begin(), types.end(), getToken().type)  != types.end()){
            if (!consumeOnly) newTokens.push_back(tokens.at(index));
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
                    std::string nextInstruction = generateUniqueLabel(macroName);
                    addGeneratedToken(TokenType::Comma, ",");
                    addGeneratedToken(TokenType::Label, nextInstruction);
                    addGeneratedToken(TokenType::Newline, "\n");
                    addGeneratedToken(TokenType::Label, nextInstruction);
                    addGeneratedToken(TokenType::Colon, ":");
                    consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                }
            }
            else{
                printError(getToken().sourceInfo, "Unknown instruction " + getToken().lexeme + "!\n");
            }
        }
        else if (match(TokenType::Label)){
            // check if the label is a macro
            auto macro = std::find_if(macros.begin(), macros.end(), [&](Macro const& m){
                return m.name == getToken().lexeme;
            });

            if (macro != macros.end()){
                consumeOnly = true;
                consumeType(TokenType::Label);
                // expand the macro
                auto bodyCopy = macro->body;
                
                // replace the macro arguments with values from the macro call
                for (int i=0; i<macro->arguments.size(); i++){
                    auto value = consumeTypes({TokenType::Label, TokenType::Address, TokenType::Number});
                    std::replace_if(bodyCopy.begin(), bodyCopy.end(), [&](Token const& t){
                        return t.type == TokenType::Label && t.lexeme == macro->arguments.at(i);
                    }, value);
                    if (i != macro->arguments.size()-1){
                        if (match(TokenType::Comma)){
                            consumeType(TokenType::Comma);
                        }
                        else{
                            printError(getToken().sourceInfo, "While expanding macro " + macro->name + ": Expected a macro argument, but got " + std::to_string(getToken()) + "!\n");
                        }
                    }
                }
                consumeTypes({TokenType::Newline, TokenType::EndOfFile});
                bodyCopy = expandTokens(bodyCopy, macros);
                auto expanded = bodyCopy;

                // uniquify the local labels
                for (int i=0; i<macro->localLabels.size(); i++){
                    std::replace_if(expanded.begin(), expanded.end(), [&](Token const& t){
                        return t.type == TokenType::Label && t.lexeme == macro->localLabels.at(i);
                    }, Token(TokenType::Label, uniqueifyLabel(macro->localLabels.at(i), macro->name)));
                }
                if (!macro->localLabels.empty()){
                    auto x = Token(TokenType::Label, uniqueifyLabel(macro->localLabels.at(0), macro->name));
                    std::to_string(x);
                }
                newTokens.insert(newTokens.end(), expanded.begin(), expanded.end());
                consumeOnly = false;
            }
            else{
                // just add the label
                consume();
            }
        }
        
        else if (match(TokenType::Keyword)){
            if (getToken().lexeme == ".macro"){
                consumeOnly = true;
                consumeType(TokenType::Keyword);
                Macro macro;
                macro.name = consumeType(TokenType::Label).lexeme;
                if (match(TokenType::Label)){
                    // parse the arguments separated by commas
                    macro.arguments.push_back(consumeType(TokenType::Label).lexeme);
                    while (match(TokenType::Comma)){
                        consumeType(TokenType::Comma);
                        macro.arguments.push_back(consumeType(TokenType::Label).lexeme);
                    }
                }
                // parse the macro body
                consumeType(TokenType::OpenBrace);
                while (!match(TokenType::CloseBrace)){
                    if (match(TokenType::Label) && matchNext(TokenType::Colon)){
                        macro.localLabels.push_back(getToken().lexeme);
                    }
                    macro.body.push_back(consume());
                }
                consumeType(TokenType::CloseBrace);
                consumeOnly = false;
                macros.push_back(macro);
            }
            else if (getToken().lexeme == ".data"){
                consumeType(TokenType::Keyword);
                consumeType(TokenType::Number);
            }
            else{
                printError(getToken().sourceInfo, "Unknown keyword " + getToken().lexeme + "!\n");
            }
        }
        else{
            consume();
        }
    }
    return newTokens;
}