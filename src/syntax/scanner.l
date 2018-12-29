%option c++
%option yyclass="Scanner"
%option prefix="c89c"
%option noyywrap

%{
#include <string>

#include "scanner.h"

typedef c89c::Parser::token token;
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

"auto"                  return token::AUTO;
"break"                 return token::BREAK;
"case"                  return token::CASE;
"char"                  return token::CHAR;
"const"                 return token::CONST;
"continue"              return token::CONTINUE;
"default"               return token::DEFAULT;
"do"                    return token::DO;
"double"                return token::DOUBLE;
"else"                  return token::ELSE;
"enum"                  return token::ENUM;
"extern"                return token::EXTERN;
"float"                 return token::FLOAT;
"for"                   return token::FOR;
"goto"                  return token::GOTO;
"if"                    return token::IF;
"int"                   return token::INT;
"long"                  return token::LONG;
"register"              return token::REGISTER;
"return"                return token::RETURN;
"short"                 return token::SHORT;
"signed"                return token::SIGNED;
"sizeof"                return token::SIZEOF;
"static"                return token::STATIC;
"struct"                return token::STRUCT;
"switch"                return token::SWITCH;
"typedef"               return token::TYPEDEF;
"union"                 return token::UNION;
"unsigned"              return token::UNSIGNED;
"void"                  return token::VOID;
"volatile"              return token::VOLATILE;
"while"                 return token::WHILE;

{L}({L}|{D})*           value->emplace<std::string>(yytext); return token::IDENTIFIER;

0[xX]{H}+{IS}?          return token::CONSTANT;
0{D}+{IS}?              return token::CONSTANT;
{D}+{IS}?               return token::CONSTANT;
L?'(\\.|[^\\'])+'       return token::CONSTANT;

{D}+{E}{FS}?            return token::CONSTANT;
{D}*"."{D}+({E})?{FS}?  return token::CONSTANT;
{D}+"."{D}*({E})?{FS}?  return token::CONSTANT;

L?\"(\\.|[^\\"])*\"     return token::STRING_LITERAL;

"..."                   return token::ELLIPSIS;
">>="                   return token::RIGHT_ASSIGN;
"<<="                   return token::LEFT_ASSIGN;
"+="                    return token::ADD_ASSIGN;
"-="                    return token::SUB_ASSIGN;
"*="                    return token::MUL_ASSIGN;
"/="                    return token::DIV_ASSIGN;
"%="                    return token::MOD_ASSIGN;
"&="                    return token::AND_ASSIGN;
"^="                    return token::XOR_ASSIGN;
"|="                    return token::OR_ASSIGN;
">>"                    return token::RIGHT_OP;
"<<"                    return token::LEFT_OP;
"++"                    return token::INC_OP;
"--"                    return token::DEC_OP;
"->"                    return token::PTR_OP;
"&&"                    return token::AND_OP;
"||"                    return token::OR_OP;
"<="                    return token::LE_OP;
">="                    return token::GE_OP;
"=="                    return token::EQ_OP;
"!="                    return token::NE_OP;
";"                     return ';';
("{"|"<%")              return '{';
("}"|"%>")              return '}';
","                     return ',';
":"                     return ':';
"="                     return '=';
"("                     return '(';
")"                     return ')';
("["|"<:")              return '[';
("]"|":>")              return ']';
"."                     return '.';
"&"                     return '&';
"!"                     return '!';
"~"                     return '~';
"-"                     return '-';
"+"                     return '+';
"*"                     return '*';
"/"                     return '/';
"%"                     return '%';
"<"                     return '<';
">"                     return '>';
"^"                     return '^';
"|"                     return '|';
"?"                     return '?';

[ \t\v\n\f]             ;
.                       {}

%%
