%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="circuit_lexer.cpp"
%option prefix="circuit_"
%{
	#include "circuit_parser.h"
	#include <string>
	using namespace std;
	#define SAVE_TOKEN Gate_lval.string = yytext;
	// #define TOKEN(t) (Gate_lval.token = t)
%}

word [[:alnum:]]+
digit [0-9]
int {digit}+
float {int}\.{int}
num {float}|{int}
whitespace [ \t\n]
comment #.*
%%

"circuit"         return CIRCUIT;
"lines"           return LINES;
"in_labels"       return INLAB;
"out_lables"      return OUTLAB;
"constant"        return CONST;
"garbage"         return GARB;
"["               return LSBRAC;
"]"               return RSBRAC;
","               return COMMA;
":"               return MAPTO;
"->"              return MAPTO;
";"							  return NEWLINE;
{word}						SAVE_TOKEN; return WORD;

{whitespace}
{comment}
%%
