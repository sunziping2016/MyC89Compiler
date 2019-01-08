#ifndef MYC89COMPILER_ERROR_H
#define MYC89COMPILER_ERROR_H

#include  <iostream>
#include <stdexcept>

namespace c89c {
    class SemanticError: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
        virtual const char *name() const {
            return "semantic error";
        }
    };

    std::ostream &operator << (std::ostream &out, const SemanticError &err);

    class SemanticWarning: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
        virtual const char *name() const {
            return "semantic warning";
        }
    };

    std::ostream &operator << (std::ostream &out, const SemanticWarning &err);
}

#endif //MYC89COMPILER_ERROR_H
