%{
	#include <string>	
	#include <iostream>
	#include "parseNode.h"
	#define YYSTYPE parseNode*
	using namespace std;
	string printTree(parseNode *node);
	void yyerror(const char *s);
	int yylex();
%}
%error-verbose
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
             | exp '\n'  {cout << printTree($1) << endl; }
     ;

     exp:      subex                     { $$ = $1; }
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


int main( int argc, char *argv[] ){
	extern FILE * yyin;
	++argv; --argc;
	yyin = fopen( argv[0], "r" );
	yyparse ();
}

string printTree(parseNode *node){
	if (node->left == NULL && node->left == NULL ) return node->value;
	return "("+printTree(node->left)+ " " +node->value + " " + printTree(node->right)+")";
}

void yyerror (const char *s){ /* Called by yyparse on error */
	printf ("%s\n", s);
}
