#pragma once

#include "ASMPass.hpp"

class BinaryGenerationPass : public ASMPass{
    public:
        BinaryGenerationPass(std::string filename, uint64_t binarySize=0);
        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) override;

        std::vector<int64_t> const& getBinary();

    private:
        std::string filename;
        
        std::vector<int64_t> binary;
        uint64_t binarySize;
};