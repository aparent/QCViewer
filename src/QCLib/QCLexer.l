%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="QCLexer.cpp"
%option prefix="QC_"
%{
	#include "QCParser.h"
	#include <iostream>
	//#define SAVE_TOKEN QC_lval.string = strdup(yytext);
	#define SAVE_TOKEN QC_lval.string = strdup(yytext); std::cout<< yytext << " ";
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

\.v                 printok("V");return TOKEN(VARS);
\.i                 printok("I");return TOKEN(INPUTS);
\.o                 printok("O");return TOKEN(OUTPUTS);
\.c                 printok("C");return TOKEN(CONSTANTS);
\.ol               	printok("OL"); return TOKEN(OUTLABELS);
"BEGIN"							printok("BEGIN");return TOKEN(START);
"END"								printok("END");return TOKEN(END);
"\n"								std::cout<<std::endl;return TOKEN(NEWLINE);
"("									printok("LBRAC");return TOKEN(LBRAC);
")"									printok("RBRAC");return TOKEN(RBRAC);
"^"									printok("EXPON");return TOKEN(EXPON);
"'"									printok("APOS");return TOKEN(APOS);
{num}								SAVE_TOKEN; return NUM;
{word}							SAVE_TOKEN; return WORD;

{whitespace}
{comment}
%%
