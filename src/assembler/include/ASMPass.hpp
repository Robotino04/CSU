#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "Token.hpp"
#include "SourceInformation.hpp"

inline std::string uniqueifyLabel(std::string const& label, std::string const& macroName){
    static uint64_t counter = 0;
    if (!macroName.empty()){
        return macroName + "_" + label + std::to_string(counter++);
    }
    else{
        return label + std::to_string(counter++);
    }
}

inline std::string generateUniqueLabel(std::string const& macroName){
    return uniqueifyLabel("label", macroName);
}

class ASMPass{
    public:
        ASMPass(){}
        virtual ~ASMPass(){}

        virtual std::vector<Token>& operator() (std::vector<Token>& tokens) = 0;

        void printError(SourceInformation info, std::string msg) const;

        void pushErrorContext(std::string context);
        void popErrorContext();

    private:
        std::vector<std::string> errorContextStack;
};