%option noyywrap
%{
	#include "parseNode.h"
	#define YYSTYPE parseNode*
	#include "diracParser.tab.h"
	#include <iostream>
	using namespace std;
%}

%x ket

%%

[0-9]+i?/[^\>]		  {
											parseNode * val = new parseNode;
											val->value = string(yytext); val->left = NULL; val->right=NULL;
											yylval = val;
											return NUM;
										}
\|									{BEGIN(ket);}
<ket>[0-1]+    	    {
											parseNode * val = new parseNode;
											val->value = "|"+string(yytext)+">"; val->left = NULL; val->right=NULL;
											yylval = val;
											return KET;
										}
<ket>\>							{BEGIN(INITIAL);}
<*>i								/*ignore the left behind i's*/
<*>[ \t]+						/* eat whitespace */
.|\n		 						{return(yytext[0]);}


%%

