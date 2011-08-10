%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="GateLexer.cpp"
%option prefix="Gate_"
%{
	#include <string>
	#define SAVE_TOKEN yylval.string = yytext
	#define TOKEN(t) (yylval.token = t)
%}

word [[:alnum:]]+
digit [0-9]
int {digit}+
float {int}\.{int}
num {float}|{int}
whitespace [ \t]
comment #.*
%%

"^"                 return TOKEN(EXPONENT);
"+"                 return TOKEN(PLUS);
"-"                 return TOKEN(MINUS);
"*"                 return TOKEN(TIMES);
"/"                 return TOKEN(DIV);
"i"									return TOKEN(IMAG);
","									return TOKEN(COMMA)
"NAME"							return TOKEN(NAME);
"SYMBOL"						return TOKEN(SYMBOL);
"SQRT"							return TOKEN(SQRT);
{num}								SAVE_TOKEN; return NUM;
{word}							SAVE_TOKEN; return WORD;

{whitespace}
{comment}
%%
