#ifndef MYC89COMPILER_SCANNER_H
#define MYC89COMPILER_SCANNER_H

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer c89cFlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL int c89c::Scanner::lex(c89c::Parser::semantic_type *value)

#include "parser.hpp"

namespace c89c {
    class Scanner: public c89cFlexLexer {
    public:
        using c89cFlexLexer::c89cFlexLexer;
        virtual int lex(Parser::semantic_type *value);
    };
}

#endif //MYC89COMPILER_SCANNER_H
