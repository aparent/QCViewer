%{
  #include <stdio.h>
  #include <string.h>
	#include <iostream>
	#include <string>
	#include "parseNode.h"
	#define YYSTYPE parseNode*
	using namespace std;
	string printTree(parseNode *node);
	void yyerror(const char *s);
	int yylex();
  parseNode *final;
%}
%error-verbose
%defines "diracParser.h"
%start input
%token CNUM NUM KET PLUS MINUS TIMES DIV SQRT /* CNUM is for complex numbers  */
%left low
%left '-' '+'
%left '*' '/'
%left SQRT
%right high
%initial-action {
};

%%
input:			 /*empty*/
             | exp                      { final = $1; }
     ;
		 exp:      subex                    { $$ = $1;    }
             | error                    { cout << "E3" <<endl; }
		 				 | exp '+' exp  				    {
																					parseNode * val = new parseNode;
																					val->value = "+"; val->left = $1; val->right=$3;
																					val->type = PLUS;
							 														$$ = val;
							 													}
		 				 | exp '-' exp  				    {
																					parseNode * val = new parseNode;
																					val->value = "-"; val->left = $1; val->right=$3;
																					val->type = MINUS;
							 														$$ = val;
							 													}
						 | exp '*' exp           	  {
																					parseNode * val = new parseNode;
																					val->value = "*"; val->left = $1; val->right=$3;
																					val->type = TIMES;
							 														$$ = val;
						 														}
						 | exp '/' exp           	  {
																					parseNode * val = new parseNode;
																					val->value = "/"; val->left = $1; val->right=$3;
																					val->type = DIV;
							 														$$ = val;
						 														}
		 ;
		 subex: term
		 				| subex subex	%prec high    {
																					parseNode * val = new parseNode;
																					val->value = "*"; val->left = $1; val->right=$2;
																					val->type = TIMES;
							 														$$ = val;
						 											 		  }
					  |  SQRT '(' exp ')' 				{
							 														$1->right = $3;
																					$$ = $1;
						 														}
             | '(' exp ')'            	{ $$ = $2; }
             |     error                { cout << "E0" <<endl; }
             | '(' error ')'            { cout << "E1" <<endl; }
		 ;

		 term: 		KET												{ $$ = $1; }
		 				 |NUM                       { $$ = $1; }
		 				 |CNUM                      { $$ = $1; }
		 				 |error 										{ cout << "E2" <<endl; }
		 ;

%%

extern int yy_scan_string(const char*);
extern void yy_delete_buffer(void);

parseNode *parseDirac(string input){
	//extern FILE * yyin;
  char *in = (char*)malloc(input.length() + 1);
  strcpy(in,input.c_str());
  //yyin = fmemopen (in, strlen (in), "r");
	yy_scan_string(in);
	yyparse ();
  return final;
}

void yyerror (const char *s){ /* Called by yyparse on error */
	printf ("%s\n", s);
}
