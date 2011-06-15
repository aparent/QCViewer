%{
	#define YYPARSE_PARAM scanner
	#define YYLEX_PARAM   scanner
  #include <stdio.h>
  #include <string.h>
  #include <iostream>
  #include <string>
	#include "QCLParserUtils.h"
	
  void yyerror(const char *s);
  using namespace std;
	QCLParseNode *final;
%}

%union{
	char* str;
	QCLParseNode * node;
}
%name-prefix "QCL"
%error-verbose
%verbose 
%defines "QCLParser.h"
%output "QCLParser.cpp"
%define api.pure
%locations

%start input
%token IF FOR PLUS MINUS TIMES DIV EQUALS INPUTS 
%token LINE OPERATION OPEXPONENT FUNC
%token <str> ID KVAR NUM KET OP
%type <node> block exp line conditional var operations finputs 
%left MINUS PLUS
%left TIMES DIV
%right EXPONENT

%%
input: 				/* empty */
							| block { final = $1; }

block:					line 
							| FOR exp'..'exp         '{' '\n' block '}' '\n' block 	{ $$ = setupFOR($2,$4,$7,$10); }
							| IF '(' conditional ')' '{' '\n' block '}' '\n' block 	{ $$ = NULL; }
              | line '\n' block	 { $$ = setupLINE($1,$3); } 
              | '\n' block  { $$ = $2; } 
		;
line:					  var EQUALS exp	{ $$ = setupBINOP(EQUALS,$1,$3); }
							| operations KVAR { $$ = setupOPERATION($1, $2);}
							| ID '(' finputs ')'
		;
var: 					  KVAR {$$=setupVAR(KVAR,$1);}
							| ID   {$$=setupVAR(ID,$1);}
		;
operations:			OP {$$=setupOP($1,NULL);}
							|	OP EXPONENT exp {$$=setupOPEXPONENT($1,$3,NULL);} 
							|	OP EXPONENT exp operations {$$=setupOPEXPONENT($1,$3,$4);}
							| OP operations {$$=setupOP($1,$2);}
		;
finputs:			/*empty*/						{$$ = NULL;}
							| KVAR ',' finputs	{$$ = setupINPUTS(setupVar($1),$3);}
							| exp  ',' finputs	{$$ = setupINPUTS($1,$3);}
							| KVAR							{$$ = setupINPUTS(setupVar($1),NULL);}
							| exp								{$$ = setupINPUTS($1,NULL);}
		;
exp:					  value							
							| exp MINUS 		exp { $$ = setupBINOP(MINUS,$1,$3);   } 
							|	exp PLUS  		exp { $$ = setupBINOP(PLUS,$1,$3);    }
							|	exp TIMES			exp { $$ = setupBINOP(TIMES,$1,$3);   }
							| exp DIV 			exp { $$ = setupBINOP(DIV,$1,$3);     }
							| exp EXPONENT 	exp { $$ = setupBINOP(EXPONENT,$1,$3);}
							| '(' exp ')'				{ $$ = $2;                        }
							| ID '(' finputs ')'
		;
value:					ID
							|	NUM
							| KET
;
conditional:  
;
%%

void QCLerror (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}
