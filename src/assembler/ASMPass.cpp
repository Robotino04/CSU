#include "ASMPass.hpp"

#include <iostream>

void ASMPass::printError(SourceInformation info, std::string msg) const{
    std::cout << info.filename << ":" << info.line << ":" << info.column << ": " << msg << "\n";

    // Print the error context stack
    for (auto it = errorContextStack.rbegin(); it != errorContextStack.rend(); it++){
        std::cout << "\twhile " << *it << "\n";
    }

    throw std::runtime_error(msg);
}

void ASMPass::pushErrorContext(std::string context){
    errorContextStack.push_back(context);
}

void ASMPass::popErrorContext(){
    errorContextStack.erase(errorContextStack.end()-1);
}