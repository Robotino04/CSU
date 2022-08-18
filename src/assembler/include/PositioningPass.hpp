#pragma once

#include "ASMPass.hpp"

class PositioningPass : public ASMPass{
    public:
        PositioningPass(std::string filename);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

        uint64_t getBinarySize();

    private:
        Token& getToken();
        Token& consumeType(TokenType type);
        Token& consumeTypes(std::vector<TokenType> types);

    private:
        std::string filename;

        std::vector<Token>* tokens;

        uint64_t pos = 0;
        int index = 0;
};