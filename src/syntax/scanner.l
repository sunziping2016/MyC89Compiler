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
#define NC column_number += yyleng
%}

%%
"/*"                    {
    char c, prev = 0;
	while ((c = yyinput()) != 0 && (c != '/' || prev != '*')) {
	    if (c == '\n') {
	        ++line_number;
	        column_number = 1;
	    } else
	        ++column_number;
		prev = c;
	}
	if (c == 0)
	    error("unterminated comment");
}

"//"[^\n]*      NC;

"#"[ \t\f]+{D}+[ \t\f]+\"(\\.|[^\\"\n])*\"([ \t\f]+[1234])*	{
    NC;
    int begin = 1, end = yyleng;
    line_number = 0;
    column_number = 1;
    while (begin < end && isspace(yytext[begin])) ++begin;
    while (begin < end && isdigit(yytext[begin]))
        line_number = line_number * 10 + (yytext[begin++] - '0');
    --line_number;
    filename.clear();
    while (begin < end && yytext[begin] != '\"') ++begin;
    ++begin;
    while (begin < end && yytext[end] != '\"') --end;
    while (begin < end) {
        char ch;
        begin = parseEscape(yytext, begin, end, ch);
        filename.push_back(ch);
    }
}

"auto"                  NC; return token::AUTO;
"break"                 NC; return token::BREAK;
"case"                  NC; return token::CASE;
"char"                  NC; return token::CHAR;
"const"                 NC; return token::CONST;
"continue"              NC; return token::CONTINUE;
"default"               NC; return token::DEFAULT;
"do"                    NC; return token::DO;
"double"                NC; return token::DOUBLE;
"else"                  NC; return token::ELSE;
"enum"                  NC; return token::ENUM;
"extern"                NC; return token::EXTERN;
"float"                 NC; return token::FLOAT;
"for"                   NC; return token::FOR;
"goto"                  NC; return token::GOTO;
"if"                    NC; return token::IF;
"int"                   NC; return token::INT;
"long"                  NC; return token::LONG;
"register"              NC; return token::REGISTER;
"return"                NC; return token::RETURN;
"short"                 NC; return token::SHORT;
"signed"                NC; return token::SIGNED;
"sizeof"                NC; return token::SIZEOF;
"static"                NC; return token::STATIC;
"struct"                NC; return token::STRUCT;
"switch"                NC; return token::SWITCH;
"typedef"               NC; return token::TYPEDEF;
"union"                 NC; return token::UNION;
"unsigned"              NC; return token::UNSIGNED;
"void"                  NC; return token::VOID;
"volatile"              NC; return token::VOLATILE;
"while"                 NC; return token::WHILE;

{L}({L}|{D})*           {
    NC;
    value->emplace<std::string>(yytext);
    return token::IDENTIFIER;
}

0[xX]{H}+{IS}?          NC; return token::CONSTANT;
0{D}+{IS}?              NC; return token::CONSTANT;
{D}+{IS}?               NC; return token::CONSTANT;
L?'(\\.|[^\\'])+'       NC; return token::CONSTANT;

{D}+{E}{FS}?            NC; return token::CONSTANT;
{D}*"."{D}+({E})?{FS}?  NC; return token::CONSTANT;
{D}+"."{D}*({E})?{FS}?  NC; return token::CONSTANT;

L?\"(\\.|[^\\"])*\"     NC; return token::STRING_LITERAL;

"..."                   NC; return token::ELLIPSIS;
">>="                   NC; return token::RIGHT_ASSIGN;
"<<="                   NC; return token::LEFT_ASSIGN;
"+="                    NC; return token::ADD_ASSIGN;
"-="                    NC; return token::SUB_ASSIGN;
"*="                    NC; return token::MUL_ASSIGN;
"/="                    NC; return token::DIV_ASSIGN;
"%="                    NC; return token::MOD_ASSIGN;
"&="                    NC; return token::AND_ASSIGN;
"^="                    NC; return token::XOR_ASSIGN;
"|="                    NC; return token::OR_ASSIGN;
">>"                    NC; return token::RIGHT_OP;
"<<"                    NC; return token::LEFT_OP;
"++"                    NC; return token::INC_OP;
"--"                    NC; return token::DEC_OP;
"->"                    NC; return token::PTR_OP;
"&&"                    NC; return token::AND_OP;
"||"                    NC; return token::OR_OP;
"<="                    NC; return token::LE_OP;
">="                    NC; return token::GE_OP;
"=="                    NC; return token::EQ_OP;
"!="                    NC; return token::NE_OP;
";"                     NC; return ';';
("{"|"<%")              NC; return '{';
("}"|"%>")              NC; return '}';
","                     NC; return ',';
":"                     NC; return ':';
"="                     NC; return '=';
"("                     NC; return '(';
")"                     NC; return ')';
("["|"<:")              NC; return '[';
("]"|":>")              NC; return ']';
"."                     NC; return '.';
"&"                     NC; return '&';
"!"                     NC; return '!';
"~"                     NC; return '~';
"-"                     NC; return '-';
"+"                     NC; return '+';
"*"                     NC; return '*';
"/"                     NC; return '/';
"%"                     NC; return '%';
"<"                     NC; return '<';
">"                     NC; return '>';
"^"                     NC; return '^';
"|"                     NC; return '|';
"?"                     NC; return '?';

[ \t\v\f]               NC;
"\n"                    ++line_number; column_number = 1;

.                       {
    NC;
    error(std::string("unexpected character \'") + *yytext + "\' (" + std::to_string((int)*yytext) + ")");
}

%%

int c89c::Scanner::parseEscape(const char *str, int begin, int end, char &result) {
    if (str[begin++] != '\\') {
        result = str[begin - 1];
        return begin;
    }
    if (begin == end) {
        error("unexpected ending of escape sequence");
        result = '\\';
        return begin;
    }
    switch (str[begin++]) {
        case 'a': result = '\a'; return begin;
        case 'b': result = '\b'; return begin;
        case 'f': result = '\f'; return begin;
        case 'n': result = '\n'; return begin;
        case 'r': result = '\r'; return begin;
        case 't': result = '\t'; return begin;
        case 'v': result = '\v'; return begin;
        case 'x':
            if (begin == end || !isxdigit(str[begin])) {
                if (begin == end)
                    error("unexpected ending of escape sequence");
                else
                    error("unknown escape sequence");
                result = 'x';
            } else {
                std::string value;
                value.push_back(str[begin++]);
                if (begin != end && isxdigit(str[begin]))
                    value.push_back(str[begin++]);
                result = static_cast<char>(std::stoi(value, nullptr, 16));
            }
            return begin;
        case '\'': case '\"': case '\?': case '\\':
            result = str[begin - 1];
            return begin;
        default:
            if ('0' <= str[begin - 1] && str[begin - 1] <= '7') {
                std::string value;
                value.push_back(str[begin - 1]);
                for (int i = 0; i < 2 && begin != end && '0' <= str[begin] && str[begin] <= '7'; ++i)
                    value.push_back(str[begin++]);
                result = static_cast<char>(std::stoi(value, nullptr, 8));
            } else {
                error("unknown escape sequence");
                result = str[begin - 1];
            }
            return begin;
    }
}