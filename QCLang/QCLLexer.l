%option noyywrap
%option nounput
%option never-interactive
%option unistd
%option outfile="QCLang/QCLLexer.cpp"
%option prefix="QCL_"
%{
	#include <string>
	#include <sstream>
  #include <iostream>
  #include <cstdlib>
	#define YYSTYPE QCLParseNode*
	#include "QCLParserUtils.h"
	using namespace std;
	stringstream QCL_temp;
%}

%x ket
%x ketmult
%x wireList

DIGIT [0-9]
INT {DIGIT}+
FLOAT {INT}\.{INT}
ID [a-z][a-zA-z0-9]*
OP [A-Z][a-z]*
%%
\^                 	{ return EXPONENT;  }
{OP}								{
											QCL_lval = setupVAR(OP,strdup(yytext));
											return OP;
										}
{ID}								{
											QCL_lval = setupVAR(ID,strdup(yytext));
											return ID;
										}
\+                  { return PLUS;  }
\-                  { return MINUS; }
\*                  { return TIMES; }
\/                  { return DIV;   }
=										{ return EQUALS;}
{FLOAT}/i						|
{INT}/i							{
											QCL_lval = setupVAR(COMPLEX,strdup(yytext));
											return COMPLEX;
										}
{INT}			      	  {
											QCL_lval = setupVAR(INT,strdup(yytext));
											return INT;
										}
{FLOAT}							{
											QCL_lval = setupVAR(FLOAT,strdup(yytext));
											return FLOAT;
										}
\|									{BEGIN(ket);}
<ket>{ID}			      {
											QCL_lval = setupVAR(KVAR,strdup(yytext));
											return KVAR;
										}
<ket>[0-1]+    	    {
											QCL_lval = setupVAR(KET,strdup(yytext));
											return KET;
										}
<ket>[0-1]+/\^      { cout << yytext << endl;
											QCL_temp.str("");
											QCL_temp << yytext;
											BEGIN(ketmult);
										}
<ket>\>							{BEGIN(INITIAL);}

<ketmult>\^
<ketmult>{DIGIT}+			{
											int reps = atoi(yytext);
											string pat = QCL_temp.str();
											for(int i = 0; i<reps-1; i++) QCL_temp << pat;
											QCL_lval = setupVAR(KET,strdup(QCL_temp.str().c_str()));
											return KET;
										}
<ketmult>\>					{BEGIN(INITIAL);}

<*>, 						/* eat commas (these are just treated as spaces by the compliler for now)*/
<*>[ i\t\n]+		/* eat whitespace */
\.\.            {return(yytext[0]);}
. 		 			    {return(yytext[0]);}
<*>#.*          /* eat comments */
%%
