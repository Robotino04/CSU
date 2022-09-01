#pragma once

#include "ASMPass.hpp"

#include <sstream>

class SourceReconstructionPass : public ASMPass{
    public:
        SourceReconstructionPass(std::string filename);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

        std::string getSource();

    private:
        std::string filename;
        std::stringstream source;
};