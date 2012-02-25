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

Authors: Alex Parent, Jacob Parker
---------------------------------------------------------------------*/

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

word [[:alnum:]]+"*"?
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
