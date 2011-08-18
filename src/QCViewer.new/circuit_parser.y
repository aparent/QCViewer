%{
  #include <string>
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include <iostream>
  void circuit_error(const char *s);
	int circuit_lex ();
	int circuit__scan_string(const char*);
	#include "circuit_parser_utils.h"
	circuit_list_t *circ_final = NULL;
	using namespace std;
%}

%code requires{
	#include "circuit_parser_utils.h"
}
%union {
	char *string;
	std::complex<float> *val;
	var_list_t *vars;
	map_list_t *maps;
	gate_list_t *gates;
	param_list_t *params;
	col_list_t *cols;
	option_list_t *options;
	circuit_list_t *circuits;
}

%name-prefix "circuit_"
%error-verbose
%verbose
%defines "circuit_parser.h"
%output "circuit_parser.cpp"

%token NEWLINE CIRCUIT WORD LINES INLAB OUTLAB MAPTO COMMA
%token NUM RPAREN LPAREN MINUS PLUS TIMES DIV EXPONENT SQRT IMAG PI

%type <string> id NUM WORD
%type <val> exp
%type <circuits> input circuit
%type <cols> cols
%type <options> options
%type <maps> maps
%type <vars> vars
%type <gates> gates
%type <params> exps

%left NEWLINE
%left MINUS PLUS
%left TIMES DIV
%right SQRT EXPONENT
%left IMAG

%%
file: input {circ_final = $1}
;
input:  /*empty*/ {$$ = NULL}
        | NEWLINE input {$$ = $2}
        | circuit input {$1->next = $2; $$ = $1}
;
circuit: CIRCUIT WORD options cols {$$ = new circuit_list_t($2,$3,$4);}
;
options: NEWLINE options 			{$$ = $2}
        | LINES vars options 	{$$ = new option_list_t("LINES",$2,$3)}
        | INLAB vars options	{$$ = new option_list_t("INLAB",$2,$3)}
        | OUTLAB vars options	{$$ = new option_list_t("OUTLAB",$2,$3)}
        | NEWLINE							{$$ = NULL}
;
cols: /*empty*/ {$$ = NULL}
      | gates NEWLINE cols {$$ = new col_list_t($1,$3)}
;
gates: WORD maps NEWLINE { $$ = new gate_list_t($1,$2,NULL)}
			| WORD LPAREN exps RPAREN maps NEWLINE { $$ = new gate_list_t($1,$5,$3,NULL)}
      | WORD maps NEWLINE gates{ $$ = new gate_list_t($1,$2,$4)}
      | WORD LPAREN exps RPAREN maps NEWLINE gates{ $$ = new gate_list_t($1,$5,$3,$7)}
			| WORD vars NEWLINE { $$ = new gate_list_t($1,$2,NULL)}
			| WORD LPAREN exps RPAREN vars NEWLINE { $$ = new gate_list_t($1,$5,$3,NULL)}
      | WORD vars NEWLINE gates{ $$ = new gate_list_t($1,$2,$4)}
      | WORD LPAREN exps RPAREN vars NEWLINE gates{ $$ = new gate_list_t($1,$5,$3,$7)}
;
vars: id {$$ = new var_list_t($1,NULL)}
			|id vars{$$ = new var_list_t($1,$2)}
;
maps:  id MAPTO id{$$ = new map_list_t($1,$3,NULL);}
      |id MAPTO id maps{$$ = new map_list_t($1,$3,$4);}
;
id: WORD {$$ = $1;}
		|NUM {$$ = $1;}
;
exps:	exp {$$ = new param_list_t(*$1,NULL); delete $1}
			| exp COMMA exps {$$ = new param_list_t(*$1,$3); delete $1}
;

exp:	  NUM							 { $$ = new complex<float>(atof($1),0);}
			| PI							 { $$ = new complex<float>(M_PI,0);}
			| IMAG             { $$ = new complex<float>(0,1);}
			| exp MINUS 	 exp { $$ = new complex<float>(*$1 - *$3); delete $1; delete $3;}
			| exp PLUS 		 exp { $$ = new complex<float>(*$1 + *$3); delete $1; delete $3;}
			| exp TIMES 	 exp { $$ = new complex<float>(*$1 * *$3); delete $1; delete $3;}
			| exp DIV      exp { $$ = new complex<float>(*$1 / *$3); delete $1; delete $3;}
			| exp EXPONENT exp { $$ = new complex<float>(pow(*$1,*$3)); delete $1; delete $3;}
			| SQRT  exp 				{ *$2 = sqrt(*$2); $$ = $2 }
			| exp IMAG					{ $$ = new complex<float>(-imag(*$1),real(*$1)); delete $1;}
			| MINUS exp 				{ *$2 = -*$2; $$ = $2}
			| LPAREN exp RPAREN	{ $$ = $2;}
;

%%

void circuit_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}

circuit_list_t *parse_circuits(string input){
	//circuit_debug = 1;
	char *in = (char*)malloc(input.length() + 1);
	strcpy(in,input.c_str());
	circuit__scan_string(in);
	circuit_parse ();
	return circ_final;
}
