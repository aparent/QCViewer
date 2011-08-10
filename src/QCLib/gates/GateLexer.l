%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="GateLexer.cpp"
%option prefix="Gate_"
%{
	#include "GateParser.h"
	#include <string>
	#include <cstdlib>
	#include <iostream>
	using namespace std;
	#define SAVE_TOKEN Gate_lval.string = strdup(yytext);
	#define TOKEN(t) (Gate_lval.token = t)
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
"("									return TOKEN(LPAREN);
")"									return TOKEN(RPAREN);
","									return TOKEN(COMMA);
"\n"								return TOKEN(NEWLINE);
"NAME"							return TOKEN(NAME);
"SYMBOL"						return TOKEN(SYMBOL);
"sqrt"							return TOKEN(SQRT);
{num}								SAVE_TOKEN; return NUM;
{word}							SAVE_TOKEN; return WORD;

{whitespace}
{comment}
%%
