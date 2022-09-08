#pragma once

#include "ASMPass.hpp"

class PositioningPass : public ASMPass{
    public:
        PositioningPass(std::string filename);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

        uint64_t getBinarySize();

    private:
        Token& getToken();
        Token const& getToken() const;
        Token& consumeType(TokenType type);
        Token& consumeTypes(std::vector<TokenType> types);
        std::vector<Token> consumeExpression(bool shouldBeAddress);
        bool match(std::vector<TokenType> types) const;

    private:
        std::string filename;

        std::vector<Token>* tokens;

        uint64_t pos = 0;
        uint64_t maxPos = 0;
        int index = 0;
};