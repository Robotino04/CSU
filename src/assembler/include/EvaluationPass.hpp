#pragma once

#include <list>

#include "ASMPass.hpp"

class EvaluationPass : public ASMPass{
    public:
        EvaluationPass(std::string filename);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

    private:
        bool isDone() const;
        Token getToken() const;
        bool match(TokenType type) const;
        bool match(std::vector<TokenType> types) const;
        Token consume();
        Token consumeType(TokenType type);
        Token consumeTypes(std::vector<TokenType> types);
        void addGeneratedToken(Token const& token);

        Token parseExpression();
        Token parseAdditiveExp();
        Token parseFactor();
        Token parseTerm();

    private:
        std::string filename;

        std::list<Token>* tokens;
        std::list<Token>::iterator tokensIt;
        bool removeTokens = false;
};