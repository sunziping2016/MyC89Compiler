#ifndef MYC89COMPILER_SCANNER_H
#define MYC89COMPILER_SCANNER_H

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer c89cFlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL int c89c::Scanner::lex(c89c::Parser::semantic_type *value)

#include <iostream>
#include <string>

#include "parser.hpp"

namespace c89c {
    struct Position {
        std::string filename;
        std::size_t line_number;
        std::size_t column_number;
    };

    inline std::ostream &operator << (std::ostream &out, const Position &pos) {
        out << pos.filename << ':' << pos.line_number << ':' << pos.column_number;
        return out;
    }

    class Scanner: public c89cFlexLexer {
    public:
        Scanner(std::istream *in, std::string filename) : c89cFlexLexer(in), filename(std::move(filename)) {}

        virtual int lex(Parser::semantic_type *value);

        Position position() {
            return { filename, line_number, column_number };
        }
    protected:
        void error(const std::string &msg) {
            std::cerr << position() << ": scanner error: " << msg << std::endl;
        }
        int parseEscape(const char *str, int begin, int end, char &result);

private:
        std::string filename;
        std::size_t line_number = 1;
        std::size_t column_number = 1;
    };
}

#endif //MYC89COMPILER_SCANNER_H
