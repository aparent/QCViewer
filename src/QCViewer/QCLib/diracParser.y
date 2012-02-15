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


%{
  #include <stdio.h>
  #include <string.h>
  #include <string>
  #include "QCLib/parseNode.h"
  #define YYSTYPE parseNode*
  using namespace std;
  string printTree(parseNode *node);
  void yyerror(const char *s);
  int yylex();
  int yy_scan_string(const char*);
  parseNode *final;
%}
%debug
%error-verbose
%verbose
%expect 4
%defines "diracParser.h"
%start input
%token CNUM NUM KET PLUS MINUS TIMES DIV SQRT /* CNUM is for complex numbers  */
%left low
%left MINUS PLUS
%left TIMES
%left DIV
%left SQRT
%left EXPON

%%
input:	/*empty*/
     		| exp 	{ final = $1; }
     		| error 	{ if ($1!=NULL) delete $1; final = NULL; }
;
exp:	subex            { $$ = $1;    }
			| exp PLUS  exp  { $2->left = $1; $2->right=$3; $$ = $2; }
      | exp MINUS exp  { $2->left = $1; $2->right=$3; $$ = $2; }
      | exp TIMES exp  { $2->left = $1; $2->right=$3; $$ = $2; }
;
subex: term
       | subex subex  %prec low      { parseNode * val = new parseNode;
                                       val->value = "*"; val->left = $1; val->right=$2; val->type = TIMES;
                                       $$ = val; }
       | subex DIV NUM               { $2->left = $1; $2->right=$3; $$ = $2; }
       | subex EXPON NUM               { $2->left = $1; $2->right=$3; $$ = $2; }
       | subex DIV '(' exp ')'       { $2->left = $1; $2->right=$4; $$ = $2; }
       | subex DIV SQRT '(' exp ')'  { $2->left = $1; $2->right=$3; $3->right = $5; $$ = $2; }
       |  SQRT '(' exp ')'           { $1->right = $3; $$ = $1; }
       | '(' exp ')'              	 { $$ = $2; }
;

term: KET    { $$ = $1; }
      |NUM   { $$ = $1; }
      |CNUM  { $$ = $1; }
;
%%

parseNode *parseDirac(string input){
  char *in = (char*)malloc(input.length() + 1);
  strcpy(in,input.c_str());
  yy_scan_string(in);
  yyparse ();
  return final;
}

void yyerror (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}
