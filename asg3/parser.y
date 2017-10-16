//Jacob Katzeff :: jkatzeff@ucsc.edu :: 1426015
//
//Chris Hsiao :: chhsiao@ucsc.edu :: 1398305
%{

#include <cassert>
#include <stdlib.h>

#include "lyutils.h"
#include "astree.h"
%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_CHAR TOK_INT TOK_STRING TOK_BOOL
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULL TOK_NEW TOK_ARRAY TOK_FALSE TOK_TRUE
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT TOK_NEWSTRING

%token TOK_FUNCTION TOK_PARAMLIST TOK_PROTOTYPE
%token TOK_DECLID TOK_INDEX TOK_RETURNVOID
%token TOK_VARDECL 

%right TOK_IF TOK_ELSE
%right '='

%left TOK_EQ TOK_GE TOK_LE TOK_NE TOK_GT TOK_LT
%left '+' '-'

%left '*' '/' '%'
%right TOK_NEG TOK_POS '!' TOK_NEW

%left TOK_ARRAY TOK_FIELD TOK_FUNCTION
%left '.' '['

%nonassoc '('

%start start

%%
start     : program { yyparse_astree = $1; }
          ;

program   : program structdef               { $$ = $1->adopt ($2); }
          | program function                { $$ = $1->adopt ($2); }
          | program stmt                    { $$ = $1->adopt ($2); }
          | program error '}'               { $$ = $1; }
          | program error ';'               { $$ = $1; }
          |                                 { $$ = new_parseroot (); }
          ;

structdef : TOK_STRUCT TOK_IDENT '{' '}'    { $$ = $1->adopt_sym_child($2,TOK_TYPEID); 
                                              destroy($3,$4); }
          | TOK_STRUCT TOK_IDENT stmts '}'  { 
                                              $$ = $1->adopt_sym_child($2,TOK_TYPEID); 
                                              $$ = $1->adopt($3); 
                                              destroy($4, nullptr); 
                                            }
          ;

stmts     :  '{' fielddecl ';'              { $$ = $1->adopt($2); 
                                              destroy($3); }
          |  stmts fielddecl ';'            { $$ = $1->adopt($2); 
                                              destroy($3); }
          ;

fielddecl : basetype TOK_ARRAY TOK_IDENT    { $$ = $2->adopt($1); $$ = $2->adopt_sym_child($3, TOK_FIELD); }
          | basetype TOK_IDENT              { $$ = $1->adopt_sym_child($2,TOK_FIELD); }
          ;

basetype  : TOK_VOID                { $$ = $1; }
          | TOK_INT                 { $$ = $1; }
          | TOK_STRING              { $$ = $1; }
          | TOK_CHAR                { $$ = $1; }
          | TOK_IDENT               { $$ = $1->adopt_sym(nullptr, TOK_TYPEID); }
          ;

function  : identdecl '(' ')' ';'     { 
                                       astree* as = new astree(TOK_PROTOTYPE,$1->lloc, "");
                                       $$ = as->adopt($1);
                                       $$ = as->adopt_sym_child($2, TOK_PARAMLIST); 
                                       destroy($3, $4); 
                                      }
          | identdecl '(' ')' block   { 
                                        astree* as = new astree(TOK_FUNCTION,$1->lloc, "");
                                        $$ = as->adopt($1); 
                                        $$ = as->adopt_sym_child($2, TOK_PARAMLIST); 
                                        $$ = as->adopt($4); 
                                        destroy($3, nullptr); 
                                      }
          | identdecl params ')' ';'   { 
                                        astree* as = new astree(TOK_PROTOTYPE,$1->lloc,"");
                                        $$ = as->adopt($1);
                                        $$ = as->adopt($2); 
                                        destroy($3, $4); 
                                       }
          | identdecl params ')' block { 
                                        astree* as = new astree(TOK_FUNCTION,$1->lloc,"");
                                        $$ = as->adopt($1);
                                        $$ = as->adopt($2);
                                        $$ = as->adopt($4); 
                                        destroy($3, nullptr); 
                                        }
          ;

params    : '(' identdecl             { $$ = $1->adopt_sym($2, TOK_PARAMLIST); }
          | params ',' identdecl      { $$ = $1->adopt($3); 
                                        destroy($2, nullptr); }
          ;

identdecl : basetype TOK_ARRAY TOK_IDENT    { $$ = $2->adopt($1);
                                              $$ = $2->adopt_sym_child($3, TOK_DECLID); }
          | basetype TOK_IDENT              { $$ = $1->adopt_sym_child($2, TOK_DECLID); }
          ;

block     : '{' '}'        { $$ = $1->adopt_sym(nullptr, TOK_BLOCK); 
                             destroy($2, nullptr); 
                           }
          | body '}'        { $$ = $1->adopt_sym(nullptr, TOK_BLOCK); 
                             destroy($2, nullptr); 
                             }
          ;

body      : '{' stmt              { $$ = $1->adopt_sym($2, TOK_BLOCK); }
          | body stmt             { $$ = $1->adopt($2); }
          ;

stmt      : block                           { $$ = $1; }
          | vardecl                         { $$ = $1; }
          | while                           { $$ = $1; }
          | ifelse                          { $$ = $1; }
          | return                          { $$ = $1; }
          | expr ';'                        { $$ = $1; 
                                              destroy($2, nullptr); }
          | ';'                             { $$ = $1; }
          ;
vardecl   : identdecl '=' expr ';'          { $$ = $2->adopt_sym
                                              ($1, TOK_VARDECL); 
                                              $$ = $2->adopt($3); 
                                              destroy($4, nullptr); }
          ;

while     : TOK_WHILE '(' expr ')' stmt     { $$ = $1->adopt($3);
                                              $$ = $1->adopt($5); 
                                              destroy($2, $4); }
          ;

ifelse    : TOK_IF '(' expr ')' stmt %prec TOK_ELSE
                                            { $$ = $1->adopt($3);
                                              $$ = $1->adopt($5); 
                                              destroy($2, $4); }
          | TOK_IF '(' expr ')' stmt TOK_ELSE stmt 
                                            { $$ = $1->adopt_sym
                                              ($3, TOK_IFELSE); 
                                              $$ = $1->adopt($5);
                                              $$ = $1->adopt($7); 
                                              destroy($2, $4); }
          ;

return    : TOK_RETURN ';'      { $$ = $1->adopt_sym
                                  (nullptr, TOK_RETURNVOID); 
                                  destroy($2, nullptr); }
          | TOK_RETURN expr ';' { $$ = $1->adopt($2); 
                                  destroy($3, nullptr); }
          ;

expr      : expr '=' expr       { $$ = $2->adopt($1, $3); }
          | expr TOK_EQ expr    { $$ = $2->adopt($1, $3); }
          | expr TOK_NE expr    { $$ = $2->adopt($1, $3); }
          | expr TOK_LT expr    { $$ = $2->adopt($1, $3); }
          | expr TOK_LE expr    { $$ = $2->adopt($1, $3); }
          | expr TOK_GT expr    { $$ = $2->adopt($1, $3); }
          | expr TOK_GE expr    { $$ = $2->adopt($1, $3); }
          | expr '+' expr       { $$ = $2->adopt($1, $3); }
          | expr '-' expr       { $$ = $2->adopt($1, $3); }
          | expr '*' expr       { $$ = $2->adopt($1, $3); }
          | expr '/' expr       { $$ = $2->adopt($1, $3); }
          | expr '%' expr       { $$ = $2->adopt($1, $3); }
          | '+' expr %prec TOK_POS
                                { $$ = $1->adopt_sym($2,TOK_POS); }
          | '-' expr %prec TOK_NEG
                                { $$ = $1->adopt_sym($2, TOK_NEG); }
          | '!' expr            { $$ = $1->adopt($2); }
          | allocator           { $$ = $1; }
          | call                { $$ = $1; }
          | variable            { $$ = $1; }
          | constant            { $$ = $1; }
          | '(' expr ')'        { $$ = $2; destroy($1, $3); }
          ;


allocator : TOK_NEW TOK_IDENT '(' ')'       { 
                                              $$ = $1->adopt_sym_child
                                              ($2, TOK_TYPEID); 
                                              destroy($3, $4);
                                            }
          | TOK_NEW TOK_STRING '(' expr ')' { 
                                              $$ = $1->adopt_sym(
                                              $4, TOK_NEWSTRING); 
                                              destroy($2,nullptr); 
                                            }
          | TOK_NEW basetype '[' expr ']'   { 
                                              $$ = $1->adopt_sym(
                                              $2, TOK_NEWARRAY); 
                                              $$ = $1->adopt($4); 
                                            }
          ;
call      : TOK_IDENT '(' ')'               { 
                                              $$ = $2->adopt_sym(
                                              $1, TOK_CALL);
                                              destroy($3, nullptr); 
                                            }
          | exprs ')'                       { $$ = $1; 
                                              destroy($2, nullptr); }
          ;

exprs     : TOK_IDENT '(' expr              { 
                                              $$ = $2->adopt_sym(
                                              $1, TOK_CALL); 
                                              $$ = $2->adopt($3); 
                                            }
          | exprs ',' expr                  { $$ = $1->adopt($3); 
                                              destroy($2, nullptr); }
          ;

variable  : TOK_IDENT                       { $$ = $1; }
          | expr '[' expr ']'               { 
                                              $$ = $2->adopt_sym(
                                              $1, TOK_INDEX); 
                                              $$ = $2->adopt($3); 
                                              destroy($4, nullptr); 
                                            }
          | expr '.' TOK_IDENT              { 
                                              $$ = $2->adopt($1);
                                              $$ = $2->adopt_sym_child(
                                              $3, TOK_FIELD);
                                            }
          ;

constant  : TOK_INTCON                      { $$ = $1; }
          | TOK_CHARCON                     { $$ = $1; }
          | TOK_STRINGCON                   { $$ = $1; }
          | TOK_NULL                        { $$ = $1; }
          ;

%%

const char *parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}

bool parser::is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}
