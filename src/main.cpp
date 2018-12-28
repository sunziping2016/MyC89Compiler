#include "syntax/parser.hpp"
#include "syntax/scanner.h"

int main() {
    c89c::Scanner scanner;
    c89c::Parser parser(scanner);
    parser.parse();
    return 0;
}