%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="diracLexer.cpp"
%option prefix="QCL"
%option bison-bridge bison-locations
%{
	#include <string>
	#include <sstream>
  #include <iostream>
  #include <cstdlib>
	#include "QCLParserUtils.h"
	using namespace std;
	stringstream temp;
%}

%x ket
%x ketmult

DIGIT [0-9]
ID [a-z][a-z0-9]*
OP [A-Z][a-z]*
%%
FOR                 { return FOR;   }
IF                 	{ return IF;    }
{OP}								{
											yylval->str = strdup(yytext);
											return OP;
										}
{ID}								{
											yylval->str = strdup(yytext);
											return ID;
										}
\+                  { return PLUS;  }
\-                  { return MINUS; }
\*                  { return TIMES; }
\/                  { return DIV;   }
=										{ return EQUALS;}
{DIGIT}+i							{
											yylval->str = strdup(yytext);
											return NUM;
										}
{DIGIT}+	      	    {
											yylval->str = strdup(yytext);
											return NUM;
										}
{DIGIT}+\.{DIGIT}+	      {
											yylval->str = strdup(yytext);
											return NUM;
										}
\|									{BEGIN(ket);}
<ket>[a-zA-Z]+      {
											yylval->str = strdup(yytext);
											return KVAR;
										}
<ket>[0-1]+    	    {
											yylval->str = strdup(yytext);
											return KET;
										}
<ket>[0-1]+/\^      { cout << yytext << endl;
											temp.str("");
											temp << yytext;
											BEGIN(ketmult);
										}
<ket>\>							{BEGIN(INITIAL);}

<ketmult>\^
<ketmult>{DIGIT}+			{
											int reps = atoi(yytext);
											string pat = temp.str();
											for(int i = 0; i<reps-1; i++) temp << pat;
											yylval->str=strdup(temp.str().c_str());
											return KET;
										}
<ketmult>\>					{BEGIN(INITIAL);}

<*>[ \t]+						/* eat whitespace */
. 		 			    	  {return(yytext[0]);}


%%
