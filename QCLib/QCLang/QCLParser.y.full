%{
	#define YYSTYPE QCLParseNode*
  #include <stdio.h>
  #include <string.h>
  #include <iostream>
  #include <string>
	#include "QCLParserUtils.h"
  void QCL_error(const char *s);
  using namespace std;
	QCLParseNode *QCL_Final;
	int QCL_lex ();
	int QCL__scan_string(const char*);
%}


%name-prefix "QCL_"
%error-verbose
%verbose 
%defines "QCLang/QCLParser.h"
%output "QCLang/QCLParser.cpp"

%start input
%token IF FOR EQUALS  
%token LINE OPERATION OPEXPONENT FUNC INPUTS
%token  ID KVAR NUM KET OP
%left MINUS PLUS
%left TIMES DIV
%right EXPONENT

%%
input: 				block { QCL_Final = $1; }

block:					/*empty*/ {$$ = NULL;} 
							| '\n' block {$$ = $2;}
              | line '\n' block	 { $$ = setupLINE($1,$3);} 
							| FOR exp'..'exp '{' '\n' block '}' block { $$ = setupFOR($2,$4,$7,$9);}
							| IF '(' conditional ')' '{' '\n' block '}'  block 	{ $$ = NULL; }
							| line {$$ = $1;}
		;
line:					  var EQUALS exp	{ $$ = setupBINOP(EQUALS,$1,$3); }
							| operations KVAR { $$ = setupOPERATION($1, $2);}
							| ID '(' finputs ')' { $$ = setupFUNC($1,$3);}
		;
var: 					  KVAR 
							| ID   
		;
operations:			OP {$$=setupOP($1,NULL);}
							|	OP EXPONENT exp {$$=setupOPEXPONENT($1,$3,NULL);} 
							|	OP EXPONENT exp operations {$$=setupOPEXPONENT($1,$3,$4);}
							| OP operations {$$=setupOP($1,$2);}
		;
finputs:			/*empty*/						{$$ = NULL;}
							| KVAR ',' finputs	{$$ = setupINPUTS($1,$3);}
							| exp  ',' finputs	{$$ = setupINPUTS($1,$3);}
							| KVAR							{$$ = setupINPUTS($1,NULL);}
							| exp								{$$ = setupINPUTS($1,NULL);}
		;
exp:					  value							
							| exp MINUS 		exp { $$ = setupBINOP(MINUS,$1,$3);   } 
							|	exp PLUS  		exp { $$ = setupBINOP(PLUS,$1,$3);    }
							|	exp TIMES			exp { $$ = setupBINOP(TIMES,$1,$3);   }
							| exp DIV 			exp { $$ = setupBINOP(DIV,$1,$3);     }
							| exp EXPONENT 	exp { $$ = setupBINOP(EXPONENT,$1,$3);}
							| '(' exp ')'				{ $$ = $2;                        }
							| ID '(' finputs ')'{ $$ = setupFUNC($1,$3);}
		;
value:					ID 
							|	NUM 
							| KET 
;
conditional:  
;
%%

void QCL_error (const char *s){ /* Called by yyparse on error */
  printf ("%s\n", s);
}

QCLParseNode *parseQCL(string input){
	char *in = (char*)malloc(input.length() + 1);
	strcpy(in,input.c_str());
	QCL__scan_string(in);
	QCL_parse ();
	return QCL_Final;
}

void printTree(QCLParseNode * node){
	cout << node->type << ":" << QCLNodeName(node->type) << ":"; 
	if (node->value != NULL){
		 cout << node->value;
	}
	cout << endl;
	for(int i = 0; i < QCLNodeNumLeaves(node->type); i++){
		if (i == 3 && node->type == FOR) cout << "ENDFOR" << endl;
		if (node->leaves[i]!= NULL){
			printTree(node->leaves[i]);
		}
	}
}
