%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="diracLexer.cpp"
%option prefix="QCL"
%option bison-bridge bison-locations
%{
	#include "parseNode.h"
	#include <string>
	#include <sstream>
  #include <iostream>
  #include <cstdlib>
	#define YYSTYPE QCLParseNode*
	#include "QCLParser.h"
	using namespace std;
	stringstream temp;
	parseNode *setupNode(int type, string value);
%}

%x ket
%x ketmult

%%
FOR                 { return FOR;   }
IF                 	{ return IF;    }
[A-Z][a-z]*					{
											yylval = setupNode(NUM,strdup(yytext));
											return OP;
										}
[a-z]+							{
											yylval = setupNode(NUM,strdup(yytext));
											return ID;
										}
\+                  { return PLUS;  }
\-                  { return MINUS; }
\*                  { return TIMES; }
\/                  { return DIV;   }
=										{ return EQUALS;}
i										{
											yylval = setupNode(NUM,strdup(yytext));
											return NUM;
										}
[0-9]+	      	    {
											yylval = setupNode(NUM,strdup(yytext));
											return NUM;
										}
[0-9]+\.[0-9]	      {
											yylval = setupNode(NUM,strdup(yytext));
											return NUM;
										}
\|									{BEGIN(ket);}
<ket>[a-zA-Z]+      {
											yylval = setupNode(KVAR,strdup(yytext));
											return KVAR;
										}
<ket>[0-1]+    	    {
											yylval = setupNode(KET,strdup(yytext));
											return KET;
										}
<ket>[0-1]+/\^      { cout << yytext << endl;
											temp.str("");
											temp << yytext;
											BEGIN(ketmult);
										}
<ket>\>							{BEGIN(INITIAL);}

<ketmult>\^
<ketmult>[0-9]+			{
											int reps = atoi(yytext);
											string pat = temp.str();
											for(int i = 0; i<reps-1; i++) temp << pat;
											yylval = setupNode(KET,temp.str());
											return KET;
										}
<ketmult>\>					{BEGIN(INITIAL);}

<*>[ \t]+						/* eat whitespace */
. 		 			    	  {return(yytext[0]);}


%%

QCLParseNode *setupNode(int type, char * value){
	parseNode * val = new parseNode;
	val->value = value; val->leaves = null;
	return val;
}
