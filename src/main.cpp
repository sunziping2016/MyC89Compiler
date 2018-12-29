#include <iostream>

#include "syntax/parser.hpp"
#include "syntax/scanner.h"

int main() {
    c89c::Scanner scanner(std::cin, std::cout);
    c89c::Parser parser(scanner);
    c89c::Parser::semantic_type s;
    std::cout << parser.parse() << std::endl;
    return 0;
}