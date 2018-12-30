#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>

#include "syntax/parser.hpp"
#include "syntax/scanner.h"

int main(int argc, char *argv[]) {
    std::string input_filename = "<stdin>";
    std::unique_ptr<std::istream> input;
    std::unique_ptr<std::ostream> output;
    int i;
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            if (++i < argc) {
                output.reset(new std::ofstream(argv[i]));
                if (!*output) {
                    std::cerr << "c89c: cannot open output file." << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "c89c: missing output file." << std::endl;
                return 1;
            }
        } else {
            if (!input) {
                input.reset(new std::ifstream(argv[i]));
                if (!*input) {
                    std::cerr << "c89c: cannot open input file." << std::endl;
                    return 1;
                }
                input_filename = argv[i];
            } else {
                std::cerr << "c89c: multiple input file." << std::endl;
                return 1;
            }
        }
    }
    c89c::Scanner scanner(input.get(), input_filename);
    c89c::Parser parser(scanner);
    parser.parse();
    return 0;
}