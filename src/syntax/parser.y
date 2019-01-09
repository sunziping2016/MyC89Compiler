%require "3.2"
%skeleton "lalr1.cc"

%define parser_class_name {Parser}
%define api.namespace {c89c}
%define api.value.type variant
%define api.value.automove

%parse-param {c89c::Scanner &scanner} {c89c::Driver &driver}

%error-verbose

%code requires
{
    #include "../ast/type.h"
    #include "../ast/declaration.h"
    #include "../ast/expression.h"

    namespace c89c {
        class Driver;
        class Scanner;
    }
}

%code top
{
    #include <memory>

    #include "../ast/error.h"
    #include "scanner.h"

    #define yylex scanner.lex

    #define BEG     try {
    #define END     } catch (SemanticError &err) {                                      \
                        std::cerr << scanner.position() << ": " << err << std::endl;    \
                        YYABORT;                                                        \
                    } catch (SemanticWarning &warn) {                                   \
                        std::cerr << scanner.position() << ": " << warn << std::endl;   \
                    }
}

%token <std::string> IDENTIFIER
%token <std::unique_ptr<c89c::Expression>> CONSTANT
%token STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN
%token <std::unique_ptr<c89c::Type>> TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <c89c::StorageClassSpecifier> storage_class_specifier
%type <c89c::TypeSpecifier> type_specifier
%type <c89c::TypeQualifier> type_qualifier
%type <c89c::TypeQualifierList> type_qualifier_list

%type <std::unique_ptr<c89c::DeclarationSpecifiers>> declaration_specifiers
%type <std::unique_ptr<c89c::Declarator>> declarator direct_declarator pointer
%type <std::unique_ptr<c89c::Expression>> primary_expression postfix_expression unary_expression
%type <std::unique_ptr<c89c::Expression>> cast_expression multiplicative_expression additive_expression
%type <std::unique_ptr<c89c::Expression>> shift_expression relational_expression equality_expression
%type <std::unique_ptr<c89c::Expression>> and_expression exclusive_or_expression inclusive_or_expression
%type <std::unique_ptr<c89c::Expression>> logical_and_expression logical_or_expression conditional_expression
%type <std::unique_ptr<c89c::Expression>> assignment_expression expression constant_expression
%type <std::unique_ptr<c89c::Initializer>> initializer
%type <std::unique_ptr<c89c::InitializerList>> initializer_list
%type <std::unique_ptr<c89c::InitDeclarator>> init_declarator
%type <std::unique_ptr<c89c::InitDeclaratorList>> init_declarator_list
%type <std::unique_ptr<c89c::Declaration>> declaration

%nonassoc THEN
%nonassoc ELSE

%start translation_unit
%%

primary_expression
	: IDENTIFIER            { $$.reset(new TodoExpression); }
	| CONSTANT              { $$ = $1; }
	| STRING_LITERAL        { $$.reset(new TodoExpression); }
	| '(' expression ')'    { $$ = $2; }
	;

postfix_expression
	: primary_expression                                    { $$ = $1; }
	| postfix_expression '[' expression ']'                 { $$.reset(new TodoExpression); }
	| postfix_expression '(' ')'                            { $$.reset(new TodoExpression); }
	| postfix_expression '(' argument_expression_list ')'   { $$.reset(new TodoExpression); }
	| postfix_expression '.' IDENTIFIER                     { $$.reset(new TodoExpression); }
	| postfix_expression PTR_OP IDENTIFIER                  { $$.reset(new TodoExpression); }
	| postfix_expression INC_OP                             { $$.reset(new TodoExpression); }
	| postfix_expression DEC_OP                             { $$.reset(new TodoExpression); }
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression                { $$ = $1; }
	| INC_OP unary_expression           { $$.reset(new TodoExpression); }
	| DEC_OP unary_expression           { $$.reset(new TodoExpression); }
	| unary_operator cast_expression    { $$.reset(new TodoExpression); }
	| SIZEOF unary_expression           { $$.reset(new TodoExpression); }
	| SIZEOF '(' type_name ')'          { $$.reset(new TodoExpression); }
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression                  { $$ = $1; }
	| '(' type_name ')' cast_expression { $$.reset(new TodoExpression); }
	;

multiplicative_expression
	: cast_expression                               { $$ = $1; }
	| multiplicative_expression '*' cast_expression { $$.reset(new TodoExpression); }
	| multiplicative_expression '/' cast_expression { $$.reset(new TodoExpression); }
	| multiplicative_expression '%' cast_expression { $$.reset(new TodoExpression); }
	;

additive_expression
	: multiplicative_expression                         { $$ = $1; }
	| additive_expression '+' multiplicative_expression { $$.reset(new TodoExpression); }
	| additive_expression '-' multiplicative_expression { $$.reset(new TodoExpression); }
	;

shift_expression
	: additive_expression                           { $$ = $1; }
	| shift_expression LEFT_OP additive_expression  { $$.reset(new TodoExpression); }
	| shift_expression RIGHT_OP additive_expression { $$.reset(new TodoExpression); }
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression    { $$ = $1; }
	| relational_expression '>' shift_expression    { $$.reset(new TodoExpression); }
	| relational_expression LE_OP shift_expression  { $$.reset(new TodoExpression); }
	| relational_expression GE_OP shift_expression  { $$.reset(new TodoExpression); }
	;

equality_expression
	: relational_expression                             { $$ = $1; }
	| equality_expression EQ_OP relational_expression   { $$.reset(new TodoExpression); }
	| equality_expression NE_OP relational_expression   { $$.reset(new TodoExpression); }
	;

and_expression
	: equality_expression                       { $$ = $1; }
	| and_expression '&' equality_expression    { $$.reset(new TodoExpression); }
	;

exclusive_or_expression
	: and_expression                                { $$ = $1; }
	| exclusive_or_expression '^' and_expression    { $$.reset(new TodoExpression); }
	;

inclusive_or_expression
	: exclusive_or_expression                               { $$ = $1; }
	| inclusive_or_expression '|' exclusive_or_expression   { $$.reset(new TodoExpression); }
	;

logical_and_expression
	: inclusive_or_expression                               { $$ = $1; }
	| logical_and_expression AND_OP inclusive_or_expression { $$.reset(new TodoExpression); }
	;

logical_or_expression
	: logical_and_expression                                { $$ = $1;  }
	| logical_or_expression OR_OP logical_and_expression    { $$.reset(new TodoExpression); }
	;

conditional_expression
	: logical_or_expression                                             { $$ = $1; }
	| logical_or_expression '?' expression ':' conditional_expression   { $$.reset(new TodoExpression); }
	;

assignment_expression
	: conditional_expression                                        { $$ = $1; }
	| unary_expression assignment_operator assignment_expression    { $$.reset(new TodoExpression); }
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression                 { $$ = $1; }
	| expression ',' assignment_expression  { $$.reset(new TodoExpression); }
	;

constant_expression
	: conditional_expression    { $$ = $1; }
	;

declaration
	: declaration_specifiers ';'                        { $$.reset(new Declaration($1)); }
	| declaration_specifiers init_declarator_list ';'   { $$.reset(new Declaration($1, $2)); }
	;

declaration_specifiers
	: storage_class_specifier                           { BEG $$->add($1);          END }
	| storage_class_specifier declaration_specifiers    { BEG $$ = $2; $$->add($1); END }
	| type_specifier                                    { BEG $$->add($1);          END }
	| type_specifier declaration_specifiers             { BEG $$ = $2; $$->add($1); END }
	| type_qualifier                                    { BEG $$->add($1);          END }
	| type_qualifier declaration_specifiers             { BEG $$ = $2; $$->add($1); END }
	;

init_declarator_list
	: init_declarator                           { $$.reset(new InitDeclaratorList); $$->add($1); }
	| init_declarator_list ',' init_declarator  { $$ = $1; $$->add($3); }
	;

init_declarator
	: declarator                    { $$.reset(new InitDeclarator($1)); }
	| declarator '=' initializer    { $$.reset(new InitDeclarator($1, $3)); }
	;

storage_class_specifier
	: TYPEDEF           { $$.set(StorageClassSpecifier::TYPEDEF); }
	| EXTERN            { $$.set(StorageClassSpecifier::EXTERN); }
	| STATIC            { $$.set(StorageClassSpecifier::STATIC); }
	| AUTO              { $$.set(StorageClassSpecifier::AUTO); }
	| REGISTER          { $$.set(StorageClassSpecifier::REGISTER); }
	;

type_specifier
	: VOID                      { $$.set(TypeSpecifier::VOID); }
	| CHAR                      { $$.set(TypeSpecifier::CHAR); }
	| SHORT                     { $$.set(TypeSpecifier::SHORT); }
	| INT                       { $$.set(TypeSpecifier::INT); }
	| LONG                      { $$.set(TypeSpecifier::LONG); }
	| FLOAT                     { $$.set(TypeSpecifier::FLOAT); }
	| DOUBLE                    { $$.set(TypeSpecifier::DOUBLE); }
	| SIGNED                    { $$.set(TypeSpecifier::SIGNED); }
	| UNSIGNED                  { $$.set(TypeSpecifier::UNSIGNED); }
	| struct_or_union_specifier { $$.set(TypeSpecifier::STRUCT_OR_UNION); }
	| enum_specifier            { $$.set(TypeSpecifier::ENUM); }
	| TYPE_NAME                 { $$.set(TypeSpecifier::TYPENAME, $1); }
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

type_qualifier
	: CONST     { $$.set(TypeQualifier::CONST); }
	| VOLATILE  { $$.set(TypeQualifier::VOLATILE); }
	;

declarator
	: pointer direct_declarator { $$ = $1; $$->setBase($2); }
	| direct_declarator         { $$ = $1; }
	;

direct_declarator
	: IDENTIFIER                                    { $$.reset(new IdentifierDeclarator($1)); }
	| '(' declarator ')'                            { $$ = $2; }
	| direct_declarator '[' constant_expression ']' { BEG $$.reset(new ArrayDeclarator($3)); $$->setBase($1); END }
	| direct_declarator '[' ']'                     { $$.reset(new ArrayDeclarator); $$->setBase($1); }
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
	;

pointer
	: '*'                               { $$.reset(new PointerDeclarator); }
	| '*' type_qualifier_list           { $$.reset(new PointerDeclarator($2)); }
	| '*' pointer                       { $$.reset(new PointerDeclarator); $$->setBase($2); }
	| '*' type_qualifier_list pointer   { $$.reset(new PointerDeclarator($2)); $$->setBase($3); }
	;

type_qualifier_list
	: type_qualifier                        { BEG $$.add($1);          END}
	| type_qualifier_list type_qualifier    { BEG $$ = $1; $$.add($2); END }
	;


parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression         { $$.reset(new ExpressionInitializer($1)); }
	| '{' initializer_list '}'      { $$ = $2; }
	| '{' initializer_list ',' '}'  { $$ = $2; }
	;

initializer_list
	: initializer                       { $$.reset(new InitializerList); $$->add($1); }
	| initializer_list ',' initializer  { $$ = $1; $$->add($3); }
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	| '{' declaration_list statement_list '}'
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement                     %prec THEN
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition
	| declaration           { BEG $1->generate(driver); END }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	| declarator declaration_list compound_statement
	| declarator compound_statement
	;

%%

void c89c::Parser::error(const std::string &msg) {
    std::cerr << scanner.position() << ": syntax error: " << msg << std::endl;
}