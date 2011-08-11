%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="diracLexer.cpp"
%{
	#include "parseNode.h"
	#include <string>
	#include <sstream>
	#include "utility.h"
	#define YYSTYPE parseNode*
	#include "diracParser.h"
	using namespace std;
	stringstream temp;
	parseNode *setupNode(int type, string value);
%}

%x ket
%x ketmult

%%
"sqrt"          		yylval = setupNode(SQRT,"SQRT"); 				return SQRT;
"+"             		yylval = setupNode(PLUS,"+"); 					return PLUS;
"-"                	yylval = setupNode(MINUS,"-");					return MINUS;
"*"                	yylval = setupNode(TIMES,"*");  				return TIMES;
"/"                 yylval = setupNode(DIV,"/");					 	return DIV;
"i"									yylval = setupNode(CNUM,"1"); 					return CNUM;
[0-9]+	      	    yylval = setupNode(NUM,string(yytext)); return NUM;
[0-9]+\.[0-9]				yylval = setupNode(NUM,string(yytext));	return NUM;
"|"									BEGIN(ket);

<ket>[0-1]+    	    yylval = setupNode(KET,string(yytext));return KET;
<ket>[0-1]+/\^      temp.str(""); temp << yytext; BEGIN(ketmult);
<ket>\>							BEGIN(INITIAL);

<ketmult>\^
<ketmult>[0-9]+			{
											int reps = atoi(yytext);
											string pat = temp.str();
											for(int i = 0; i<reps-1; i++) temp << pat;
											yylval = setupNode(KET,temp.str());
											return KET;
										}
<ketmult>\>					BEGIN(INITIAL);

<*>[ \t\n]+					/* eat whitespace */
. 		 			    	  return(yytext[0]);


%%

parseNode *setupNode(int type, string value){
	parseNode * val = new parseNode;
	val->value = value; val->left = NULL; val->right=NULL;
	val->type=type;
	return val;
}

