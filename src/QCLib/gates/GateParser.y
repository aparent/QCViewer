%{
  #include <stdlib.h>
  #include <iostream>
  #include <string>
  #include <cstdlib>
  #include "gates/GateParserUtils.h"
  void Gate_error(const char *s);
	GateParseNode *QCL_Final;
	int Gate_lex ();
	int Gate__scan_string(const char*);
%}

%union {
	std::complex<float> *val;
	char *string;
	row_terms *terms;
	matrix_row *rows;
	gate_node      *gates;
	int token;
}


%name-prefix "Gate_"
%error-verbose
%verbose
%defines "GateParser.h"
%output "GateParser.cpp"

%token NAME WORD NUM COMMA
%token SYMBOL

%left MINUS PLUS
%left TIMES DIV
%right SQRT EXPONENT
%left IMAG

%type <val> exp NUM
%type <word> WORD
%type <gates> input gate
%type <rows> matrix
%type <terms> row

%%
input:  /* empty */  {$$ = NULL;}
        | input gate {$2->next = $1; $$ = $2;}
;

gate:     NAME WORD '\n' SYMBOL WORD '\n' matrix {$$ = new gate_node(String($2), String($5), $7);}
;

matrix:		  row '\n' matrix {$$ = new martix_row($1,$3);}
					| '\n'{$$ = NULL;}
;

row: 			  exp '\n'			{$$ = $1;}
					| exp COMMA row
;
exp:					  NUM								{ $$ = new complex<float>(atof($1),0); delete $1;}
							| exp MINUS 		exp { $$ = new complex<float>(*$1 - *$3); delete $1; delete $3;}
							| exp PLUS 			exp { $$ = new complex<float>(*$1 + *$3); delete $1; delete $3;}
							| exp TIMES 		exp { $$ = new complex<float>(*$1 * *$3); delete $1; delete $3;}
							| exp DIV		 		exp { $$ = new complex<float>(*$1 / *$3); delete $1; delete $3;}
							| exp EXPONENT  exp { $$ = new complex<float>(pow(*$1,*$3)); delete $1; delete $3;}
							| SQRT  exp 				{ $$ = new complex<float>(sqrt(*$2)); delete $2;}
							| exp IMAG					{ $$ = new complex<float>(-imag(*$1),real(*$1)); delete $1;}
							| '(' exp ')'				{ $$ = $2;}
;
%%

void Gate_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}

void *parseGates(string input){
	char *in = (char*)malloc(input.length() + 1);
	strcpy(in,input.c_str());
	Gate__scan_string(in);
	Gate_parse ();
}
