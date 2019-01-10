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

#include "../ast/driver.h"
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
        Scanner(std::istream *in, std::string filename, Driver &driver):
            c89cFlexLexer(in), m_filename(std::move(filename)), m_driver(driver) {}

        virtual int lex(Parser::semantic_type *value);

        Position position() {
            return { m_filename, m_line_number, m_column_number };
        }
    protected:
        void error(const std::string &msg) {
            std::cerr << position() << ": lexical error: " << msg << std::endl;
        }
        int parseEscape(const char *str, int begin, int end, char &result);

private:
        std::string m_filename;
        std::size_t m_line_number = 1;
        std::size_t m_column_number = 1;

        Driver &m_driver;
    };
}

#endif //MYC89COMPILER_SCANNER_H
