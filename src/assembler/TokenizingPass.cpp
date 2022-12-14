#include "TokenizingPass.hpp"
#include "keywords.hpp"
#include "Token.hpp"

#include <algorithm>
#include <fstream>

TokenizingPass::TokenizingPass(std::string filename){
    this->filename = filename;

    pushErrorContext("reading source file");

    // Read the file into a string
    std::ifstream infile(filename);
    if (!infile.is_open()){
        printError({0, 0, filename}, "Could not open file \"" + filename + "\"");
    }

    std::stringstream buffer;
    buffer << infile.rdbuf();
    source = buffer.str();
    infile.close();
    popErrorContext();
}

bool TokenizingPass::isDone() const{
    return charIdx >= source.size();
}
char TokenizingPass::getChar() const{
    if (isDone())
        return '\0';
    return source[charIdx];
}
bool TokenizingPass::matches(const char chr) const{
    return getChar() == chr;
}
char TokenizingPass::consume(){
    pos.column++;
    if (charIdx > 0 && source[charIdx-1] == '\n'){
        pos.line++;
        pos.column = 0;
    }
    return source[charIdx++];
}
std::string TokenizingPass::consumeUntil(const char end){
    std::string chrs;
    while (!matches(end)){
        chrs += consume();
    }
    return chrs;
}

static bool charInRange(char chr, char start, char end){
    return chr >= start && chr <= end;
}

// defines a Single Character Token
#define SCT(TYPE, CHR) case CHR: token.type = TokenType::TYPE; token.lexeme = consume(); break

std::vector<Token>& TokenizingPass::operator() (std::vector<Token>& tokens){
    pos = {1, 0, filename};
    pushErrorContext("tokenizing file \"" + filename + "\"");

    tokens.emplace_back(TokenType::BeginOfFile, "");
    while (!isDone()){
        Token token(TokenType::None, "");
        
        token.sourceInfo = pos;
        switch (getChar()){
            case '/':
                token.type = TokenType::Slash;
                token.lexeme = consume();
                if (getChar() == '/') token.lexeme += consume();
                else break;
            case ';':
                pushErrorContext("parsing comment");
                token.type = TokenType::Comment;
                token.lexeme += consumeUntil('\n');
                popErrorContext();
                break;
            SCT(Colon, ':');
            SCT(Comma, ',');
            SCT(Newline, '\n');
            SCT(OpenBrace, '{');
            SCT(CloseBrace, '}');
            SCT(Plus, '+');
            SCT(Minus, '-');
            SCT(Star, '*');
            SCT(OpenParen, '(');
            SCT(CloseParen, ')');
                
            case '$':
                pushErrorContext("parsing address");
                token.lexeme = consume();
                if (!(std::isdigit(getChar()) || getChar() == '-' || getChar() == '+')){
                    printError(pos, "'$' must be followed by a number!");
                }
                popErrorContext();
            case '0'...'9':{
                pushErrorContext("parsing number");
                bool isAddress = token.lexeme.size() && token.lexeme.at(0) == '$';
                token.lexeme = token.lexeme.substr(isAddress ? 1 : 0);
                
                token.type = TokenType::Number;
                token.lexeme += consume();

                int base = 10;

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
                    auto c = consume();
                    if (base == 8 && !charInRange(c, '0', '7')){
                        printError(pos, "Invalid octal number!");
                    }
                    else if (base == 16 && !(charInRange(c, '0', '9') || charInRange(c, 'a', 'f') || charInRange(c, 'A', 'F'))){
                        printError(pos, "Invalid hexadecimal number!");
                    }
                    else if (base == 2 && !charInRange(c, '0', '1')){
                        printError(pos, "Invalid binary number!");
                    }
                    else if (base == 10 && !charInRange(c, '0', '9')){
                        printError(pos, "Invalid decimal number!");
                    }
                    token.lexeme += c;
                }
                bool failed = false;
                try{
                    token.data = (int64_t)std::stoll(token.lexeme, nullptr, base);
                }
                catch(std::invalid_argument){
                    failed = true;
                }
                catch(std::out_of_range){
                    failed = true;
                }

                if (failed){
                    printError(pos, token.lexeme + " is not a valid number!");
                }

                if (isAddress){
                    token.lexeme.insert(token.lexeme.begin(), '$');
                    token.type = TokenType::Address;
                    token.data = static_cast<uint16_t>(std::any_cast<int64_t>(token.data));
                }
                else{
                    token.type = TokenType::Number;
                    token.data = static_cast<int8_t>(std::any_cast<int64_t>(token.data));
                }
                popErrorContext();
                break;
            }
            case '"':{
                pushErrorContext("parsing string");
                token.lexeme = consume();
                token.type = TokenType::String;

                bool escaping = false;
                std::string actualString = "";
                while (!matches('"') && !isDone()){
                    if (escaping){
                        // escape characters from https://en.wikipedia.org/wiki/Escape_sequences_in_C
                        switch (getChar()){
                            case 'a': actualString += '\a'; break;
                            case 'b': actualString += '\b'; break;
                            case 'e': actualString += '\e'; break;
                            case 'f': actualString += '\f'; break;
                            case 'n': actualString += '\n'; break;
                            case 't': actualString += '\t'; break;
                            case 'v': actualString += '\v'; break;
                            case 'r': actualString += '\r'; break;
                            case '\\': actualString += '\\'; break;
                            case '\'': actualString += '\''; break;
                            case '"': actualString += '"'; break;
                            case '?': actualString += '?'; break;

                            default:
                                printError(pos, std::string("Unexpectdly escaped character '") + getChar() + "'!");
                                break;
                        }
                        token.lexeme += consume();
                    }
                    else if (matches('\\')){
                        escaping = true;
                        token.lexeme += consume();
                    }
                    else{
                        const char c = consume();
                        actualString += c;
                        token.lexeme += c;
                    }
                }
                
                if (!matches('"'))
                    printError(pos, "Expected '\"' after string, but hit end of file!");
                token.lexeme += consume();

                token.data = actualString;
                popErrorContext();
                break;
            }
            case 'a'...'z':
            case 'A'...'Z':
            case '.':
            case '_':
                pushErrorContext("parsing label, instruction, or keyword");
                token.lexeme = consume();
                while (!isDone() && (std::isalnum(getChar()) || getChar() == '_')){
                    token.lexeme += consume();
                }
                if (std::find(instructions.begin(), instructions.end(), token.lexeme) != instructions.end())
                    token.type = TokenType::Instruction;
                
                else if (std::find(keywords.begin(), keywords.end(), token.lexeme) != keywords.end())
                    token.type = TokenType::Keyword;
                else if (token.lexeme.at(0) == '.')
                    printError(pos, "Invalid label name or keyword \"" + token.lexeme + "\"!");
                else
                    token.type = TokenType::Label;
                popErrorContext();
                break;

            default:
                if (std::isspace(getChar())){
                    consume();
                    break;
                }
                printError(pos, std::string("Unknown character \"") + getChar() + "\"");
                consume();
        }
        if (token.type != TokenType::None)
            tokens.push_back(token);
    }
    tokens.emplace_back(TokenType::EndOfFile, "\\0");
    
    popErrorContext();
    return tokens;
}