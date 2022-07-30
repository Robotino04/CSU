#include "parsingFunctions.hpp"
#include "keywords.hpp"
#include "Token.hpp"

#include <algorithm>

std::vector<Token> tokenise(std::string source, std::string filename){
    std::vector<Token> tokens;
    int charIdx = 0;
    SourceInformation pos;
    pos.line = 1;
    pos.column = 0;
    pos.filename = filename;

    const auto isDone = [&](){
        return charIdx >= source.size();
    };
    const auto getChar = [&](){
        if (isDone())
            return '\0';
        return source[charIdx];
    };
    const auto matches = [&](const char chr){
        return source[charIdx] == chr;
    };
    const auto consume = [&](){
        pos.column++;
        if (charIdx > 0 && source[charIdx-1] == '\n'){
            pos.line++;
            pos.column = 0;
        }
        return source[charIdx++];
    };
    const auto consumeUntil = [&](const char end){
        std::string chrs;
        while (!matches(end)){
            chrs += consume();
        }
        return chrs;
    };

    while (!isDone()){
        Token token(TokenType::None, "");
        
        token.sourceInfo = pos;
        switch (getChar()){
            case ';':
                token = {TokenType::Comment, consumeUntil('\n')};
                break;
            case ':':
                token = {TokenType::Colon, consume()};
                break;
            case ',':
                token = {TokenType::Comma, consume()};
                break;
            case '\n':
                consume();
                token = {TokenType::Newline, "\\n"};
                break;
            case '$':
                token.lexeme = consume();
                if (!(std::isdigit(getChar()) || getChar() == '-' || getChar() == '+')){
                    printError(pos, "'$' must be followed by a number!\n");
                    exit(1);
                }
            case '-':
            case '+':
                token.lexeme += consume();
                if (!std::isdigit(getChar())){
                    printError(pos, token.lexeme + "'+' and '-' must be followed by a number!\n");
                    exit(1);
                }
            case '0'...'9':{
                bool isAddress = token.lexeme.size() && token.lexeme.at(0) == '$';
                token.lexeme = token.lexeme.substr(isAddress ? 1 : 0);
                
                token.type = TokenType::Number;
                token.lexeme += consume();

                int base = 0;

                // the number is octal
                if (token.lexeme.at(0) == '0' && std::isalnum(getChar()))
                    base = 8;
                switch(getChar()){
                    case 'b':
                    case 'B':
                        // the number is binary
                        base = 2;
                        token.lexeme += consume();
                        break;
                    case 'X':
                    case 'x':
                        // the number is hexadecimal
                        base = 16;
                        token.lexeme += consume();
                        break;
                }
                while (!isDone() && std::isalnum(getChar())){
                    token.lexeme += consume();
                }
                bool failed = false;
                try{
                    token.data = std::stoi(token.lexeme, nullptr, base);
                }
                catch(std::invalid_argument){
                    failed = true;
                }
                catch(std::out_of_range){
                    failed = true;
                }

                if (failed){
                    printError(pos, token.lexeme + " is not a valid number!\n");
                    exit(1);
                }

                if (isAddress){
                    token.lexeme.insert(token.lexeme.begin(), '$');
                    token.type = TokenType::Address;
                }

                break;
            }
            case 'a'...'z':
            case 'A'...'Z':
            case '.':
            case '_':
                token.lexeme = consume();
                while (!isDone() && (std::isalnum(getChar()) || getChar() == '_')){
                    token.lexeme += consume();
                }
                if (std::find(instructions.begin(), instructions.end(), token.lexeme) != instructions.end())
                    token.type = TokenType::Instruction;
                
                else if (std::find(keywords.begin(), keywords.end(), token.lexeme) != keywords.end())
                    token.type = TokenType::Keyword;
                
                else
                    token.type = TokenType::Label;
                break;


            default:
                if (std::isspace(getChar())){
                    consume();
                    break;
                }
                printError(pos, std::string("Unknown character \"") + getChar() + "\"\n");
                consume();
        }
        if (token.type != TokenType::None)
            tokens.push_back(token);
    }
    tokens.emplace_back(TokenType::EndOfFile, "\\0");
    return tokens;
}