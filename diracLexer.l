%option noyywrap
%option nounput
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
	parseNode *setupNode(int type, string value);
%}

%x ket

%%
sqrt                {
											yylval = setupNode(SQRT,"SQRT");
											return SQRT;
										}
\+                  {
											yylval = setupNode(PLUS,"+");
											return PLUS;
										}
\-                  {
											yylval = setupNode(MINUS,"-");
											return MINUS;
										}
\*                  {
											yylval = setupNode(TIMES,"*");
											return TIMES;
										}
\/                  {
											yylval = setupNode(DIV,"/");
											return DIV;
										}
i										{
											yylval = setupNode(CNUM,"1");
											return CNUM;
										}
[0-9]+	      	    {
											yylval = setupNode(NUM,string(yytext));
											return NUM;
										}
\|									{BEGIN(ket);}
<ket>[0-1]+    	    {
											yylval = setupNode(KET,string(yytext));
											return KET;
										}
<ket>\>							{BEGIN(INITIAL);}
<*>[ \t\n]+					/* eat whitespace */
. 		 			    	  {return(yytext[0]);}


%%

parseNode *setupNode(int type, string value){
	parseNode * val = new parseNode;
	val->value = value; val->left = NULL; val->right=NULL;
	val->type=type;
	return val;
}

