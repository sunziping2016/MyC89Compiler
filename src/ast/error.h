#ifndef MYC89COMPILER_ERROR_H
#define MYC89COMPILER_ERROR_H

#include <iostream>
#include <stdexcept>

namespace c89c {
    class SemanticError: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
        virtual const char *name() const {
            return "semantic error";
        }
    };

    inline std::ostream &operator << (std::ostream &out, const SemanticError &err) {
        out << err.name() << ": " << err.what();
        return out;
    }

    class SemanticWarning: public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
        virtual const char *name() const {
            return "semantic warning";
        }
    };

    inline std::ostream &operator << (std::ostream &out, const SemanticWarning &err) {
        out << err.name() << ": " << err.what();
        return out;
    }
}

#endif //MYC89COMPILER_ERROR_H
