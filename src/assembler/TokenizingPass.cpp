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
    return source[charIdx] == chr;
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

std::vector<Token>& TokenizingPass::operator() (std::vector<Token>& tokens){
    pos = {1, 0, filename};
    pushErrorContext("tokenizing file \"" + filename + "\"");
    while (!isDone()){
        Token token(TokenType::None, "");
        
        token.sourceInfo = pos;
        switch (getChar()){
            case ';':
                pushErrorContext("parsing comment");
                token = {TokenType::Comment, consumeUntil('\n')};
                popErrorContext();
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
            case '{':
                consume();
                token = {TokenType::OpenBrace, "{"};
                break;
            case '}':
                consume();
                token = {TokenType::CloseBrace, "}"};
                break;
            case '$':
                pushErrorContext("parsing address");
                token.lexeme = consume();
                if (!(std::isdigit(getChar()) || getChar() == '-' || getChar() == '+')){
                    printError(pos, "'$' must be followed by a number!");
                    exit(1);
                }
                popErrorContext();
            case '-':
            case '+':
                pushErrorContext("parsing signed number");
                token.lexeme += consume();
                if (!std::isdigit(getChar())){
                    printError(pos, token.lexeme + "'+' and '-' must be followed by a number!");
                    exit(1);
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
                        exit(1);
                    }
                    else if (base == 16 && !charInRange(c, '0', '9') && !charInRange(c, 'a', 'f') && !charInRange(c, 'A', 'F')){
                        printError(pos, "Invalid hexadecimal number!");
                        exit(1);
                    }
                    else if (base == 2 && !charInRange(c, '0', '1')){
                        printError(pos, "Invalid binary number!");
                        exit(1);
                    }
                    else if (base == 10 && !charInRange(c, '0', '9')){
                        printError(pos, "Invalid decimal number!");
                        exit(1);
                    }
                    token.lexeme += c;
                }
                bool failed = false;
                try{
                    token.data = (int64_t)std::stoi(token.lexeme, nullptr, base);
                }
                catch(std::invalid_argument){
                    failed = true;
                }
                catch(std::out_of_range){
                    failed = true;
                }

                if (failed){
                    printError(pos, token.lexeme + " is not a valid number!");
                    exit(1);
                }

                if (isAddress){
                    token.lexeme.insert(token.lexeme.begin(), '$');
                    token.type = TokenType::Address;
                    token.data = (uint64_t)std::any_cast<int64_t>(token.data);
                }
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