%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="GateLexer.cpp"
%option prefix="Gate_"
%{
	#include "GateParser.h"
	#include <cstdlib>
	#define SAVE_TOKEN Gate_lval.string = strdup(yytext)
	#define TOKEN(t) (Gate_lval.token = t)
%}

word [[:alnum:]]+
digit [0-9]
int {digit}+
float {int}\.{int}
num {float}|{int}
whitespace [ \t]
comment #.*

%x TEXT
%%


"^"                 return TOKEN(EXPONENT);
"+"                 return TOKEN(PLUS);
"-"                 return TOKEN(MINUS);
"*"                 return TOKEN(TIMES);
"/"                 return TOKEN(DIV);
"i"									return TOKEN(IMAG);
"("									return TOKEN(LPAREN);
")"									return TOKEN(RPAREN);
","									return TOKEN(COMMA);
"\n"								return TOKEN(NEWLINE);
"NAME"							BEGIN(TEXT); return TOKEN(NAME); 
"SYMBOL"						BEGIN(TEXT); return TOKEN(SYMBOL); 
<TEXT>{word}				BEGIN(INITIAL); SAVE_TOKEN; return WORD;
"sqrt"							return TOKEN(SQRT);
{num}								SAVE_TOKEN; return NUM;


<*>{whitespace}
<*>{comment}
%%
