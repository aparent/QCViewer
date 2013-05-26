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

%{
  #include <stdlib.h>
  #include <stdio.h>
  #include <string>
  #include <string.h>
  #include "gates/GateParserUtils.h"
  void Gate_error(const char *s);
	int Gate_lex ();
	int Gate__scan_string(const char*);
	gate_node *gate_final;
	using namespace std;
%}

%code requires{
  #include "gates/GateParserUtils.h"
}
%union {
	std::complex<float> *val;
	char *string;
	row_terms *terms;
	matrix_row *rows;
	gate_node *gates;
	int token;
}

%name-prefix "Gate_"
%error-verbose
%verbose
%defines "GateParser.h"
%output "GateParser.cpp"

%token NAME DRAWNAME LATEXNAME WORD NUM COMMA NEWLINE LPAREN RPAREN
%token SYMBOL COST

%left MINUS PLUS
%left TIMES DIV
%right SQRT EXPONENT
%left IMAG

%type <val> exp
%type <string> WORD NUM
%type <gates> input gate
%type <rows> matrix
%type <terms> row

%%
input: 		 /* empty */  {$$ = NULL;}
        	| input NEWLINE {$$ = $1; gate_final = $1;}
        	| input gate {$2->next = $1; $$ = $2; gate_final = $2;}
;

gate:      NAME WORD NEWLINE SYMBOL WORD NEWLINE matrix {$$ = new gate_node($2, $5, $7);}
         | NAME WORD NEWLINE DRAWNAME WORD NEWLINE SYMBOL WORD NEWLINE matrix {$$ = new gate_node($2, $5, $8, $10);}
         | NAME WORD NEWLINE DRAWNAME WORD NEWLINE LATEXNAME WORD NEWLINE SYMBOL WORD NEWLINE matrix {$$ = new gate_node($2, $5, $8, $11,$13);}
;

matrix:		  row NEWLINE matrix {$$ = new matrix_row($1,$3);}
					| NEWLINE {$$ = NULL;}
;

row:  exp {$$ = new row_terms($1);}
    | exp COMMA row {$$ = new row_terms($1,$3);}
;
exp: NUM { $$ = new complex<float>(atof($1),0);}
    | IMAG { $$ = new complex<float>(0,1);}
    | exp MINUS exp { $$ = new complex<float>(*$1 - *$3); delete $1; delete $3;}
    | exp PLUS exp { $$ = new complex<float>(*$1 + *$3); delete $1; delete $3;}
    | exp TIMES exp { $$ = new complex<float>(*$1 * *$3); delete $1; delete $3;}
    | exp DIV exp { $$ = new complex<float>(*$1 / *$3); delete $1; delete $3;}
    | exp EXPONENT  exp { $$ = new complex<float>(pow(*$1,*$3)); delete $1; delete $3;}
    | SQRT  exp { *$2 = sqrt(*$2); $$ = $2; }
    | exp IMAG { $$ = new complex<float>(-imag(*$1),real(*$1)); delete $1;}
    | MINUS exp { *$2 = -*$2; $$ = $2;}
    | LPAREN exp RPAREN	{ $$ = $2;}
;
%%

void Gate_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}

gate_node *parse_gates(string input){
	char *in = (char*)malloc(input.length() + 1);
	strcpy(in,input.c_str());
	Gate__scan_string(in);
	free(in);
	Gate_parse ();
	return gate_final;
}
