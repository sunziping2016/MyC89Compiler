#include "error.h"

std::ostream &c89c::operator<<(std::ostream &out, const SemanticError &err) {
    out << err.name() << ": " << err.what();
    return out;
}

std::ostream &c89c::operator<<(std::ostream &out, const SemanticWarning &err) {
    out << err.name() << ": " << err.what();
    return out;
}
