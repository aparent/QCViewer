%option noyywrap
%option never-interactive
%option unistd
%option outfile="diracLexer.cpp"
%{
	#include "parseNode.h"
	#include <string>
  #include <iostream>
	#define YYSTYPE parseNode*
	#include "diracParser.h"
	using namespace std;
%}

%x ket

%%
sqrt                { 
											parseNode * val = new parseNode;
											val->value = "SQRT"; val->left = NULL; val->right=NULL;
											val->type=SQRT;
											yylval = val;
											return SQRT;
										}
i										{ 
											parseNode * val = new parseNode;
											val->value = "1"; val->left = NULL; val->right=NULL;
											val->type=CNUM;
											yylval = val;
											return CNUM;
										}
[0-9]+	      	    {
											parseNode * val = new parseNode;
											val->value = string(yytext); val->left = NULL; val->right=NULL;
											val->type=NUM;
											yylval = val;
											return NUM;
										}
\|									{BEGIN(ket);}
<ket>[0-1]+    	    {
											parseNode * val = new parseNode;
											val->value = string(yytext); val->left = NULL; val->right=NULL;
											val->type = KET;
											yylval = val;
											return KET;
										}
<ket>\>							{BEGIN(INITIAL);}
<*>[ \t\n]+					/* eat whitespace */
. 		 			    	  {return(yytext[0]);}


%%

