%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="QCLexer.cpp"
%option prefix="QC_"
%{
	#include "QCParser.h"
	#include "utility.h"
	#define SAVE_TOKEN QC_lval.string = strdup(sToUpper(yytext).c_str());
	#define TOKEN(t) (QC_lval.token = t)
	#define printok(t) std::cout<<t<<" ";
%}

word [[:alnum:]]+
digit [0-9]
int {digit}+
float {int}\.{int}
num {float}|{int}
whitespace [ \t]
comment #.*
%%

\.v                 return TOKEN(VARS);
\.i                 return TOKEN(INPUTS);
\.o                 return TOKEN(OUTPUTS);
\.c                 return TOKEN(CONSTANTS);
\.ol               	return TOKEN(OUTLABELS);
"BEGIN"							return TOKEN(START);
"END"								return TOKEN(END);
"\n"								return TOKEN(NEWLINE);
"("									return TOKEN(LBRAC);
")"									return TOKEN(RBRAC);
"^"									return TOKEN(EXPON);
"'"									return TOKEN(APOS);
{num}								SAVE_TOKEN; return NUM;
{word}							SAVE_TOKEN; return WORD;

{whitespace}
{comment}
%%
