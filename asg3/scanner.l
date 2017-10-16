%{
#include <stdlib.h>
#include "string_set.h"
#include "auxlib.h"
#include "lyutils.h"
#include "astree.h"
#define YY_USER_ACTION  { lexer::advance(); }
int yylval_token (int symbol) {
	if(!parser::is_defined_token(symbol)){
		fprintf(stderr, "Invalid token used.");
		exit(EXIT_FAILURE);
	}
	yylval = new astree (symbol, lexer::lloc, yytext);
	fprintf (token_file,"%zu %zu.%zu %d %s (%s) \n", 
			lexer::lloc.filenr-7, lexer::lloc.linenr, 
			lexer::lloc.offset, symbol,
			parser::get_tname(symbol),yytext);
	return symbol;
}

%}

%option 8bit
%option debug
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option verbose
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]

MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)

STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
INTEGER         ({DIGIT}+)
CHAR            ('([^\\'\n]|\\[\\'"0nt])')

BAD_IDENT 		{DIGIT}+{LETTER}({LETTER}|{DIGIT})*
BAD_CHAR 		('([^\\'\n]|\\[\\'"0nt]?)[^']?)
BAD_STRING      (\"([^\\"\n]|\\[\\'"0nt])*[^\"]?)

%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"void"          { return yylval_token (TOK_VOID); }
"char" 			{ return yylval_token (TOK_CHAR); }
"if"            { return yylval_token (TOK_IF); }
"else"          { return yylval_token (TOK_ELSE); }
"while"         { return yylval_token (TOK_WHILE); }
"return"        { return yylval_token (TOK_RETURN); }
"int"           { return yylval_token (TOK_INT); }
"string"        { return yylval_token (TOK_STRING); }
"struct"        { return yylval_token (TOK_STRUCT); }
"null"          { return yylval_token (TOK_NULL); }
"new"           { return yylval_token (TOK_NEW); }

"=="            { return yylval_token (TOK_EQ); }
"!="            { return yylval_token (TOK_NE); }
"<="            { return yylval_token (TOK_LE); }
">="            { return yylval_token (TOK_GE); }
"<"             { return yylval_token (TOK_LT); }
">"             { return yylval_token (TOK_GT); }

{IDENT}         { return yylval_token (TOK_IDENT); }
{CHAR}          { return yylval_token (TOK_CHARCON); }
{STRING}        { return yylval_token (TOK_STRINGCON); }
{INTEGER}       { return yylval_token (TOK_INTCON); }


"[]"            { return yylval_token (TOK_ARRAY); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
";"             { return yylval_token (';'); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
"%"             { return yylval_token ('%'); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"             { return yylval_token ('%'); }
"!"             { return yylval_token ('!'); }

{BAD_IDENT} 	{ lexer::badtoken (yytext); }
{BAD_STRING} 	{ lexer::badtoken (yytext); }
{BAD_CHAR} 		{ lexer::badtoken (yytext); }

.               { lexer::badchar (*yytext); }

%%
