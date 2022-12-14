#pragma once

#include <list>

#include "ASMPass.hpp"

class ExpandingPass : public ASMPass{
    public:
        ExpandingPass(std::string filename);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

    private:
        bool isDone() const;
        Token getToken() const;
        Token getNextToken() const;
        bool matchNext(TokenType type) const;
        bool match(TokenType type) const;
        bool match(std::vector<TokenType> types) const;
        Token consume();
        Token consumeType(TokenType type);
        Token consumeTypes(std::vector<TokenType> types);
        void addGeneratedToken(TokenType type, std::string const& lexeme);
        std::vector<Token> consumeExpressionTypes();

    private:
        std::string filename;

        std::vector<Macro> macros;
        std::string currentMacroName;

        std::list<Token>* tokens;
        std::list<Token>::iterator tokensIt;
        bool removeTokens = false;
};