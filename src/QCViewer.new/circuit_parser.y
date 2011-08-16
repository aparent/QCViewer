%{
  #include <string>
  void circuit_error(const char *s);
	int circuit_lex ();
	int circuit__scan_string(const char*);
	gate_node *gate_final;
	using namespace std;
%}

%code requires{
}
%union {
	std::string string;
}

%name-prefix "circuit_"
%error-verbose
%verbose
%defines "circuit_parser.h"
%output "circuit_parser.cpp"

%token NEWLINE CIRCUIT WORD LINE INLAB OUTLAB CONST GARBAGE RSBRAC LSBRAC MAPTO 

%left NEWLINE

%%
input:  /*empty*/
        | input NEWLINE
        | input circuit 
;
circuit: CIRCUIT WORD options cols
;
options: NEWLINE options 
        | LINE vars options         
        | INLAB maps options
        | OUTLAB maps options
        | CONST maps options
        | GARBAGE vars options
        | NEWLINE
;
cols: /*empty*/ 
      | gates NEWLINE cols
;
gates: gate vars NEWLINE
      | gates gate vars NEWLINE
;
gate: WORD
      |WORD LSBRAC vars RSBRAC
;
vars: /*empty*/
      |WORD vars
;
maps: /*empty*/
      |WORD MAPTO WORD maps
;
%%

void circuit_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}
/*
gate_node *parse_gates(string input){
	char *in = (char*)malloc(input.length() + 1);
	strcpy(in,input.c_str());
	Gate__scan_string(in);
	Gate_parse ();
	return gate_final;
}
*/
