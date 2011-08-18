%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="circuit_lexer.cpp"
%option prefix="circuit_"
%{
	#include "circuit_parser.h"
	#include <string>
	#include <iostream>
	using namespace std;
	#define SAVE_TOKEN circuit_lval.string = strdup(yytext)
%}

word [[:alnum:]]+
digit [0-9]
int {digit}+
float {int}\.{int}
num {float}|{int}
whitespace [ \t]
comment #.*\n
%%
	/*General*/
"\n"					return NEWLINE;
	/*Circuit*/
"circuit" 		return CIRCUIT;
"\.lines"       return LINES;
"\.in_labels"   return INLAB;
"\.out_lables"	return OUTLAB;
","           return COMMA;
"->"          return MAPTO;
	/*Expression*/
"^"						return EXPONENT;
"+"           return PLUS;
"-"           return MINUS;
"*"          	return TIMES;
"/"           return DIV;
"i"						return IMAG;
"("						return LPAREN;
")"						return RPAREN;
"sqrt"				return SQRT;
"pi"  				return PI;

{num}					SAVE_TOKEN; return NUM;
{word}				SAVE_TOKEN; return WORD;

{whitespace}
{comment}
%%
