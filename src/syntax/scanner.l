%option c++
%option yyclass="Scanner"
%option prefix="c89c"

%{
#include "scanner.h"

#define yyterminate() return c89c::Parser::token_type::END
%}

D                       [0-9]
L                       [a-zA-Z_]
H                       [a-fA-F0-9]
E                       [Ee][+-]?{D}+
FS                      (f|F|l|L)
IS                      (u|U|l|L)*

%{
%}

%%
"/*"                    {}

"auto"                  {}
"break"                 {}
"case"                  {}
"char"                  {}
"const"                 {}
"continue"              {}
"default"               {}
"do"                    {}
"double"                {}
"else"                  {}
"enum"                  {}
"extern"                {}
"float"                 {}
"for"                   {}
"goto"                  {}
"if"                    {}
"int"                   {}
"long"                  {}
"register"              {}
"return"                {}
"short"                 {}
"signed"                {}
"sizeof"                {}
"static"                {}
"struct"                {}
"switch"                {}
"typedef"               {}
"union"                 {}
"unsigned"              {}
"void"                  {}
"volatile"              {}
"while"                 {}

{L}({L}|{D})*           {}

0[xX]{H}+{IS}?          {}
0{D}+{IS}?              {}
{D}+{IS}?               {}
L?'(\\.|[^\\'])+'       {}

{D}+{E}{FS}?            {}
{D}*"."{D}+({E})?{FS}?  {}
{D}+"."{D}*({E})?{FS}?  {}

L?\"(\\.|[^\\"])*\"     {}

"..."                   {}
">>="                   {}
"<<="                   {}
"+="                    {}
"-="                    {}
"*="                    {}
"/="                    {}
"%="                    {}
"&="                    {}
"^="                    {}
"|="                    {}
">>"                    {}
"<<"                    {}
"++"                    {}
"--"                    {}
"->"                    {}
"&&"                    {}
"||"                    {}
"<="                    {}
">="                    {}
"=="                    {}
"!="                    {}
";"                     {}
("{"|"<%")              {}
("}"|"%>")              {}
","                     {}
":"                     {}
"="                     {}
"("                     {}
")"                     {}
("["|"<:")              {}
("]"|":>")              {}
"."                     {}
"&"                     {}
"!"                     {}
"~"                     {}
"-"                     {}
"+"                     {}
"*"                     {}
"/"                     {}
"%"                     {}
"<"                     {}
">"                     {}
"^"                     {}
"|"                     {}
"?"                     {}

[ \t\v\n\f]             {}
.                       {}

%%

int c89cFlexLexer::yywrap() {
    return 1;
}