#pragma once

#include "ASMPass.hpp"

class LabelResolutionPass : public ASMPass{
    public:
        LabelResolutionPass(std::string filename);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

    private:
        std::string filename;
};