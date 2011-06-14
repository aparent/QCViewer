%{
	#define YYPARSE_PARAM scanner
	#define YYLEX_PARAM   scanner
  #include <stdio.h>
  #include <string.h>
  #include <iostream>
  #include <string>
  #include "parseNode.h"
  #define YYSTYPE QCLParseNode*
  string printTree(parseNode *node);
  void yyerror(const char *s);
  int QCLlex();
  int yy_scan_string(const char*);
  parseNode *final;

  using namespace std;
%}
%name-prefix "QCL"
%error-verbose
%verbose 
%defines "QCLParser.h"
%output "QCLParser.cpp"
%define api.pure
%locations

%start block
%token NUM KET PLUS MINUS TIMES DIV EQUALS FUNC
%token IF FOR ID OP KVAR
%left MINUS PLUS
%left TIMES DIV
%right EXPONENT

%%

block:				/* empty */
							| FOR exp'..'exp '        {' '\n' block '}' '\n' block
							| IF '(' conditional ')' '{' '\n' block '}' '\n' block
              | statement '\n' block     
              | '\n' block     
		;
statement:		  var EQUALS exp
							| operations KVAR
							| ID '(' finputs ')'
		;
var: 					  KVAR
							| ID
		;
operations:			OP
							|	OP EXPONENT exp 
							|	OP EXPONENT exp operations
							| OP operations
		;
finputs:			/*empty*/
							| KVAR ',' finputs
							| exp  ',' finputs
		;
exp:					  value
							| exp MINUS 		exp 
							|	exp PLUS  		exp
							|	exp TIMES			exp
							| exp DIV 			exp
							| exp EXPONENT 	exp
							| '(' exp ')'
							| ID '(' finputs ')'
		;
value:					ID
							|	NUM
							| KET
;
conditional:  
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
