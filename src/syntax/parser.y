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

    typedef std::unique_ptr<c89c::DeclarationSpecifiers> DeclarationSpecifiersPtrType;
}

%token STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN
%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token <std::string> IDENTIFIER
%token <std::unique_ptr<c89c::Expression>> CONSTANT
%token <std::unique_ptr<c89c::Type>> TYPE_NAME

%type <c89c::StorageClassSpecifier> storage_class_specifier
%type <c89c::TypeSpecifier> type_specifier
%type <c89c::TypeQualifier> type_qualifier
%type <c89c::TypeQualifierList> type_qualifier_list

%type <std::unique_ptr<c89c::DeclarationSpecifiers>> declaration_specifiers
%type <std::unique_ptr<c89c::Declarator>> declarator direct_declarator pointer
%type <std::unique_ptr<c89c::Declarator>> abstract_declarator direct_abstract_declarator
%type <std::unique_ptr<c89c::Expression>> primary_expression postfix_expression unary_expression
%type <std::unique_ptr<c89c::Expression>> cast_expression multiplicative_expression additive_expression
%type <std::unique_ptr<c89c::Expression>> shift_expression relational_expression equality_expression
%type <std::unique_ptr<c89c::Expression>> and_expression exclusive_or_expression inclusive_or_expression
%type <std::unique_ptr<c89c::Expression>> logical_and_expression logical_or_expression conditional_expression
%type <std::unique_ptr<c89c::Expression>> assignment_expression expression constant_expression
%type <std::unique_ptr<c89c::Initializer>> initializer
%type <std::unique_ptr<c89c::InitializerList>> initializer_list
%type <std::unique_ptr<c89c::InitDeclarator>> init_declarator
%type <std::unique_ptr<c89c::ParameterDeclaration>> parameter_declaration
%type <std::unique_ptr<c89c::ParameterList>> parameter_type_list parameter_list

%nonassoc THEN
%nonassoc ELSE

%start translation_unit
%%

primary_expression
	: IDENTIFIER            { BEG $$ = std::make_unique<TodoExpression>(); END }
	| CONSTANT              { BEG $$ = $1; END }
	| STRING_LITERAL        { BEG $$ = std::make_unique<TodoExpression>(); END }
	| '(' expression ')'    { BEG $$ = $2; END }
	;

postfix_expression
	: primary_expression                                    { BEG $$ = $1; END }
	| postfix_expression '[' expression ']'                 { BEG $$ = std::make_unique<TodoExpression>(); END }
	| postfix_expression '(' ')'                            { BEG $$ = std::make_unique<TodoExpression>(); END }
	| postfix_expression '(' argument_expression_list ')'   { BEG $$ = std::make_unique<TodoExpression>(); END }
	| postfix_expression '.'                                { BEG driver.setWantIdentifier(true); END }
	  IDENTIFIER                                            { BEG driver.setWantIdentifier(false); $$ = std::make_unique<TodoExpression>(); END }
	| postfix_expression PTR_OP                             { BEG driver.setWantIdentifier(true); END }
	  IDENTIFIER                                            { BEG driver.setWantIdentifier(true); $$ = std::make_unique<TodoExpression>(); END }
	| postfix_expression INC_OP                             { BEG $$ = std::make_unique<TodoExpression>(); END }
	| postfix_expression DEC_OP                             { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression                { BEG $$ = $1; END }
	| INC_OP unary_expression           { BEG $$ = std::make_unique<TodoExpression>(); END }
	| DEC_OP unary_expression           { BEG $$ = std::make_unique<TodoExpression>(); END }
	| unary_operator cast_expression    { BEG $$ = std::make_unique<TodoExpression>(); END }
	| SIZEOF unary_expression           { BEG $$ = std::make_unique<TodoExpression>(); END }
	| SIZEOF '(' type_name ')'          { BEG $$ = std::make_unique<TodoExpression>(); END }
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
	: unary_expression                  { BEG $$ = $1; END }
	| '(' type_name ')' cast_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

multiplicative_expression
	: cast_expression                               { BEG $$ = $1; END }
	| multiplicative_expression '*' cast_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	| multiplicative_expression '/' cast_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	| multiplicative_expression '%' cast_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

additive_expression
	: multiplicative_expression                         { BEG $$ = $1; END }
	| additive_expression '+' multiplicative_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	| additive_expression '-' multiplicative_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

shift_expression
	: additive_expression                           { BEG $$ = $1; END }
	| shift_expression LEFT_OP additive_expression  { BEG $$ = std::make_unique<TodoExpression>(); END }
	| shift_expression RIGHT_OP additive_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

relational_expression
	: shift_expression                              { BEG $$ = $1; END }
	| relational_expression '<' shift_expression    { BEG $$ = std::make_unique<TodoExpression>(); END }
	| relational_expression '>' shift_expression    { BEG $$ = std::make_unique<TodoExpression>(); END }
	| relational_expression LE_OP shift_expression  { BEG $$ = std::make_unique<TodoExpression>(); END }
	| relational_expression GE_OP shift_expression  { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

equality_expression
	: relational_expression                             { BEG $$ = $1; END }
	| equality_expression EQ_OP relational_expression   { BEG $$ = std::make_unique<TodoExpression>(); END }
	| equality_expression NE_OP relational_expression   { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

and_expression
	: equality_expression                       { BEG $$ = $1; END }
	| and_expression '&' equality_expression    { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

exclusive_or_expression
	: and_expression                                { BEG $$ = $1; END }
	| exclusive_or_expression '^' and_expression    { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

inclusive_or_expression
	: exclusive_or_expression                               { BEG $$ = $1; END }
	| inclusive_or_expression '|' exclusive_or_expression   { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

logical_and_expression
	: inclusive_or_expression                               { BEG $$ = $1; END }
	| logical_and_expression AND_OP inclusive_or_expression { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

logical_or_expression
	: logical_and_expression                                { BEG $$ = $1;  END }
	| logical_or_expression OR_OP logical_and_expression    { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

conditional_expression
	: logical_or_expression                                             { BEG $$ = $1; END }
	| logical_or_expression '?' expression ':' conditional_expression   { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

assignment_expression
	: conditional_expression                                        { BEG $$ = $1; END }
	| unary_expression assignment_operator assignment_expression    { BEG $$ = std::make_unique<TodoExpression>(); END }
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
	: assignment_expression                 { BEG $$ = $1; END }
	| expression ',' assignment_expression  { BEG $$ = std::make_unique<TodoExpression>(); END }
	;

constant_expression
	: conditional_expression    { BEG $$ = $1; END }
	;

declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	;

declaration_specifiers
	: storage_class_specifier                           { BEG $$ = std::make_unique<DeclarationSpecifiers>(); $$->add($1); END }
	| storage_class_specifier declaration_specifiers    { BEG $$ = $2; $$->add($1); END }
	| type_specifier                                    { BEG $$ = std::make_unique<DeclarationSpecifiers>(); $$->add($1); END }
	| type_specifier declaration_specifiers             { BEG $$ = $2; $$->add($1); END }
	| type_qualifier                                    { BEG $$ = std::make_unique<DeclarationSpecifiers>(); $$->add($1); END }
	| type_qualifier declaration_specifiers             { BEG $$ = $2; $$->add($1); END }
	;

init_declarator_list
	: init_declarator                           { BEG $1->generate(*$<DeclarationSpecifiersPtrType>0, driver); END }
	| init_declarator_list ',' init_declarator  { BEG $3->generate(*$<DeclarationSpecifiersPtrType>0, driver); END }
	;

init_declarator
	: declarator                    { BEG $$ = std::make_unique<InitDeclarator>($1); END }
	| declarator '=' initializer    { BEG $$ = std::make_unique<InitDeclarator>($1, $3); END }
	;

storage_class_specifier
	: TYPEDEF           { BEG $$.set(StorageClassSpecifier::TYPEDEF); END }
	| EXTERN            { BEG $$.set(StorageClassSpecifier::EXTERN); END }
	| STATIC            { BEG $$.set(StorageClassSpecifier::STATIC); END }
	| AUTO              { BEG $$.set(StorageClassSpecifier::AUTO); END }
	| REGISTER          { BEG $$.set(StorageClassSpecifier::REGISTER); END }
	;

type_specifier
	: VOID                      { BEG $$.set(TypeSpecifier::VOID); END }
	| CHAR                      { BEG $$.set(TypeSpecifier::CHAR); END }
	| SHORT                     { BEG $$.set(TypeSpecifier::SHORT); END }
	| INT                       { BEG $$.set(TypeSpecifier::INT); END }
	| LONG                      { BEG $$.set(TypeSpecifier::LONG); END }
	| FLOAT                     { BEG $$.set(TypeSpecifier::FLOAT); END }
	| DOUBLE                    { BEG $$.set(TypeSpecifier::DOUBLE); END }
	| SIGNED                    { BEG $$.set(TypeSpecifier::SIGNED); END }
	| UNSIGNED                  { BEG $$.set(TypeSpecifier::UNSIGNED); END }
	| struct_or_union_specifier { BEG $$.set(TypeSpecifier::STRUCT_OR_UNION); END }
	| enum_specifier            { BEG $$.set(TypeSpecifier::ENUM); END }
	| TYPE_NAME                 { BEG $$.set(TypeSpecifier::TYPENAME, $1); END }
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER        { BEG driver.setWantIdentifier(false); END }
	  '{' struct_declaration_list '}'
	| struct_or_union                   { BEG driver.setWantIdentifier(false); END }
	  '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER        { BEG driver.setWantIdentifier(false); END }
	;

struct_or_union
	: STRUCT    { BEG driver.setWantIdentifier(true); END }
	| UNION     { BEG driver.setWantIdentifier(true); END }
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
	: enum_specifier_start '{' enumerator_list '}'              { BEG driver.setWantIdentifier(false); END }
	| enum_specifier_start IDENTIFIER '{' enumerator_list '}'   { BEG driver.setWantIdentifier(false); END }
	| enum_specifier_start IDENTIFIER                           { BEG driver.setWantIdentifier(false); END }
	;

enum_specifier_start
    : ENUM              { BEG driver.setWantIdentifier(true); END }
    ;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '='        { BEG driver.setWantIdentifier(false); END }
	  constant_expression   { BEG driver.setWantIdentifier(true); END }
	;

type_qualifier
	: CONST     { BEG $$.set(TypeQualifier::CONST); END }
	| VOLATILE  { BEG $$.set(TypeQualifier::VOLATILE); END }
	;

declarator
	: pointer direct_declarator { BEG $$ = $1; $$->setBase($2); END }
	| direct_declarator         { BEG $$ = $1; END }
	;

direct_declarator
	: IDENTIFIER                                    { BEG $$ = std::make_unique<IdentifierDeclarator>($1); END }
	| '(' declarator ')'                            { BEG $$ = $2; END }
	| direct_declarator '[' constant_expression ']' { BEG $$ = std::make_unique<ArrayDeclarator>($3); $$->setBase($1); END }
	| direct_declarator '[' ']'                     { BEG $$ = std::make_unique<ArrayDeclarator>(); $$->setBase($1); END }
	| direct_declarator '(' parameter_type_list ')' { BEG $$ = std::make_unique<FunctionDeclarator>($3); $$->setBase($1); END }
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'                     { BEG $$ = std::make_unique<FunctionDeclarator>(); $$->setBase($1); END }
	;

pointer
	: '*'                               { BEG $$ = std::make_unique<PointerDeclarator>(); END }
	| '*' type_qualifier_list           { BEG $$ = std::make_unique<PointerDeclarator>($2); END }
	| '*' pointer                       { BEG $$ = std::make_unique<PointerDeclarator>(); $$->setBase($2); END }
	| '*' type_qualifier_list pointer   { BEG $$ = std::make_unique<PointerDeclarator>($2); $$->setBase($3); END }
	;

type_qualifier_list
	: type_qualifier                        { BEG $$.add($1);          END }
	| type_qualifier_list type_qualifier    { BEG $$ = $1; $$.add($2); END }
	;


parameter_type_list
	: parameter_list                { BEG $$ = $1; END }
	| parameter_list ',' ELLIPSIS   { BEG $$ = $1; $$->setVarArgs(true);  END }
	;

parameter_list
	: parameter_declaration                     { BEG $$ = std::make_unique<ParameterList>(); $$->add($1); END }
	| parameter_list ',' parameter_declaration  { BEG $$ = $1; $$->add($3); END }
	;

parameter_declaration
	: declaration_specifiers declarator             { BEG $$ = std::make_unique<ParameterDeclaration>($1, $2); END }
	| declaration_specifiers abstract_declarator    { BEG $$ = std::make_unique<ParameterDeclaration>($1, $2); END }
	| declaration_specifiers                        { BEG $$ = std::make_unique<ParameterDeclaration>($1); END }
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
	: pointer                               { BEG $$ = $1; $$->setBase(std::make_unique<AbstractDeclarator>()); END }
	| direct_abstract_declarator            { BEG $$ = $1; END }
	| pointer direct_abstract_declarator    { BEG $$ = $1; $$->setBase($2); END }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'                               { BEG $$ = $2; END }
	| '[' ']'                                                   { BEG $$ = std::make_unique<ArrayDeclarator>(); $$->setBase(std::make_unique<AbstractDeclarator>()); END }
	| '[' constant_expression ']'                               { BEG $$ = std::make_unique<ArrayDeclarator>($2); $$->setBase(std::make_unique<AbstractDeclarator>()); END }
	| direct_abstract_declarator '[' ']'                        { BEG $$ = std::make_unique<ArrayDeclarator>(); $$->setBase($1); END }
	| direct_abstract_declarator '[' constant_expression ']'    { BEG $$ = std::make_unique<ArrayDeclarator>($3); $$->setBase($1); END }
	| '(' ')'                                                   { BEG $$ = std::make_unique<FunctionDeclarator>(); $$->setBase(std::make_unique<AbstractDeclarator>()); END }
	| '(' parameter_type_list ')'                               { BEG $$ = std::make_unique<FunctionDeclarator>($2); $$->setBase(std::make_unique<AbstractDeclarator>()); END }
	| direct_abstract_declarator '(' ')'                        { BEG $$ = std::make_unique<FunctionDeclarator>(); $$->setBase($1); END }
	| direct_abstract_declarator '(' parameter_type_list ')'    { BEG $$ = std::make_unique<FunctionDeclarator>($3); $$->setBase($1); END }
	;

initializer
	: assignment_expression         { BEG $$ = std::make_unique<ExpressionInitializer>($1); END }
	| '{' initializer_list '}'      { BEG $$ = $2; END }
	| '{' initializer_list ',' '}'  { BEG $$ = $2; END }
	;

initializer_list
	: initializer                       { BEG $$ = std::make_unique<InitializerList>(); $$->add($1); END }
	| initializer_list ',' initializer  { BEG $$ = $1; $$->add($3); END }
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
	: IF '(' expression ')' statement %prec THEN
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
	| declaration
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