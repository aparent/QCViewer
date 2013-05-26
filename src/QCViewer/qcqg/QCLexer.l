/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011  Alex Parent and The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

See also 'ADDITIONAL TERMS' at the end of the included LICENSE file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

QCViewer is a trademark of the of the The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

Authors: Alex Parent, Jacob Parker, Marc Burns
---------------------------------------------------------------------*/

%option noyywrap
%option nounput
%option never-interactive
%option outfile="QCLexer.cpp"
%option prefix="QC_"
%{
	#include "QCParser.h"
	#include "utility.h"
	#include <iostream>
	#define SAVE_TOKEN {/*std::cout << yytext << std::endl;*/ QC_lval.string = strdup(yytext);}
	#define REMOVE_QUOTES_SAVE_TOKEN {std::string s = yytext; s.erase(s.begin()); s.erase(s.end()-1); QC_lval.string = strdup(s.c_str());}
	#define TOKEN(t) (QC_lval.token = t)
	#define printok(t) std::cout<<t<<" "<<std::endl
%}

word [[:alnum:]\*]+
string \"([^\\\"]|\\.)*\"
digit [0-9]
int {digit}+
float {int}\.{int}
num {float}|{int}
whitespace [ \t,]
comment #.*
%%

\.v                 return TOKEN(VARS);
\.i                 return TOKEN(INPUTS);
\.o                 return TOKEN(OUTPUTS);
\.c                 return TOKEN(CONSTANTS);
\.ol               	return TOKEN(OUTLABELS);
"BEGIN"							return TOKEN(START);
"END"								return TOKEN(END);
"pi"								return TOKEN(PI);
"\n"								return TOKEN(NEWLINE);
";"	   							return TOKEN(NEWLINE);
":"	   							return TOKEN(COLON);
"("									return TOKEN(LBRAC);
")"									return TOKEN(RBRAC);
"^"									return TOKEN(EXPON);
"'"									return TOKEN(APOS);
"-"									return TOKEN(NEG);
"+"									return TOKEN(PLUS);
"|"									return TOKEN(BAR);
"/"									return TOKEN(DIV);
{num}								SAVE_TOKEN; return NUM;
{string}						REMOVE_QUOTES_SAVE_TOKEN; return WORD;
{word}							SAVE_TOKEN; return WORD;

{whitespace}
{comment}
%%
