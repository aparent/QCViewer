%{
	#include <string>	
	#include <iostream>
  #include <stdio.h>
  #include <string.h>
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
%token NUM KET
%left '-' '+'
%left '*'
%initial-action {
};

%%
input:			 /*empty*/
             | input line
     ;

     line:   '\n'
             | exp '\n'  {final = $1; }
     ;

     exp:      subex                     { $$ = $1;}
		 				 | exp '+' exp  				     {
																					parseNode * val = new parseNode;
																					val->value = "+"; val->left = $1; val->right=$3;
							 														$$ = val;
							 													}
		 				 | exp '-' exp  				    {
																					parseNode * val = new parseNode;
																					val->value = "-"; val->left = $1; val->right=$3;
							 														$$ = val;
							 													}
             | '(' exp ')'              { $$ = $2;}
		 ;
		 subex: term
		 				| subex subex	%prec '*'     {
																					parseNode * val = new parseNode;
																					val->value = "*"; val->left = $1; val->right=$2;
							 														$$ = val;
						 													  }
						| subex '*' subex           {
																					parseNode * val = new parseNode;
																					val->value = "*"; val->left = $1; val->right=$3;
							 														$$ = val;
						 														}
             | '(' exp ')'            	{ $$ = $2;}
		 ;
		 term: 		KET												{ $$ = $1; }
		 				 |NUM                       { $$ = $1; }
		 ;

%%


parseNode *parseDirac(string input){
	extern FILE * yyin;
  char *in = (char*)malloc(input.length() + 1);
  strcpy(in,input.c_str());
  yyin = fmemopen (in, strlen (in), "r");
	yyparse ();
  return final;
}

string printTree(parseNode *node){
	if (node->left == NULL && node->right == NULL ) return node->value;
	return "("+printTree(node->left)+ " " +node->value + " " + printTree(node->right)+")";
}

void yyerror (const char *s){ /* Called by yyparse on error */
	printf ("%s\n", s);
}
