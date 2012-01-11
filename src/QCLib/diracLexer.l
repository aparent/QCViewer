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

Authors: Alex Parent, Jakub Parker
---------------------------------------------------------------------*/


%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="diracLexer.cpp"
%{
	#include "parseNode.h"
	#include <string>
	#include <sstream>
	#include "utility.h"
	#define YYSTYPE parseNode*
	#include "diracParser.h"
	using namespace std;
	stringstream temp;
	parseNode *setupNode(int type, string value);
%}

%x ket
%x ketmult

%%
"sqrt"			yylval = setupNode(SQRT,"SQRT"); 	return SQRT;
"+"			yylval = setupNode(PLUS,"+"); 		return PLUS;
"-"			yylval = setupNode(MINUS,"-");		return MINUS;
"*"			yylval = setupNode(TIMES,"*");  	return TIMES;
"/"			yylval = setupNode(DIV,"/");		return DIV;
"^"			yylval = setupNode(EXPON,"^");		return EXPON;
"i"			yylval = setupNode(CNUM,"1"); 		return CNUM;
[0-9]+			yylval = setupNode(NUM,string(yytext)); return NUM;
[0-9]+\.[0-9]		yylval = setupNode(NUM,string(yytext));	return NUM;
"|"			BEGIN(ket);

<ket>[0-1]+		yylval = setupNode(KET,string(yytext));	return KET;
<ket>\>			BEGIN(INITIAL);

<*>[ \t\n]+		/* eat whitespace */
.			return(yytext[0]);


%%

parseNode *setupNode(int type, string value){
	parseNode * val = new parseNode;
	val->value = value; val->left = NULL; val->right=NULL;
	val->type=type;
	return val;
}

