#pragma once

#include "ASMPass.hpp"

class TokenizingPass : public ASMPass{
    public:
        TokenizingPass(std::string filename);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

    private:
        std::string filename;

        bool isDone() const;
        char getChar() const;
        bool matches(const char chr) const;
        char consume();
        std::string consumeUntil(const char end);


        int charIdx = 0;
        SourceInformation pos;
        std::string source;
};