#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include "ast/driver.h"
#include "syntax/parser.hpp"
#include "syntax/scanner.h"

int main(int argc, char *argv[]) {
    std::string input_filename = "<stdin>";
    std::unique_ptr<std::istream> input;
    std::unique_ptr<llvm::raw_fd_ostream> output;
    int i;
    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            if (++i < argc) {
                std::error_code err;
                output.reset(new llvm::raw_fd_ostream(argv[i], err, llvm::sys::fs::F_Text));
                if (err) {
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
    c89c::Driver driver(input_filename);
    c89c::Scanner scanner(input.get(), input_filename);
    c89c::Parser parser(scanner, driver);
    parser.parse();
    if (output)
        driver.output(*output);
    else
        driver.output(llvm::outs());
    return 0;
}