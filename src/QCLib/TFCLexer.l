%option outfile="TFCLexer.cpp"
%option c++
%option noyywrap
%option noyyalloc
%option noyyrealloc
%option noyyfree
%option never-interactive
%option unistd
%{
	#include "TFCLexer.h"
	#include "utility.h"
	#include <stdlib.h>
	vector<TFCToken> *tokens = new vector<TFCToken>;
%}

%x lineNames
%x lineQoute
%x gates
%x gateVars
%x gateQoute
%x gateSet

WHITESPACE [[:blank:]\t]+
COMMENT #.*
NAME [[:alnum:]]+

%%
\.v           					{tokens->push_back(TFCToken(SEC_START,"V"));     BEGIN(lineNames);}
\.i           					{tokens->push_back(TFCToken(SEC_START,"I"));     BEGIN(lineNames);}
\.o           					{tokens->push_back(TFCToken(SEC_START,"O"));     BEGIN(lineNames);}
\.c          					  {tokens->push_back(TFCToken(SEC_START,"C"));     BEGIN(lineNames);}
\.(ol)									{tokens->push_back(TFCToken(SEC_START,"OL"));    BEGIN(lineNames);}
(?i:begin)							{tokens->push_back(TFCToken(SEC_START,"GATES")); BEGIN(gates);    }
\n

<lineNames>{NAME}				           {tokens->push_back(TFCToken(VAR_NAME,yytext));}
<lineNames>\"							         {BEGIN(lineQoute);}
<lineNames>,
<lineNames>\n											 {BEGIN(INITIAL);}
<lineQoute>[[:alnum:][:space:]]*   {tokens->push_back(TFCToken(VAR_NAME,yytext));}
<lineQoute>\"											 {BEGIN(lineNames);}

<gates>(?i:end)         					{tokens->push_back(TFCToken(SEC_END,"GATES")); BEGIN(INITIAL);}
<gates>{NAME}		       						{tokens->push_back(TFCToken(VAR_NAME,sToUpper(yytext)));}
<gates>\(													{BEGIN(gateSet);}
<gates>[ ]           							{BEGIN(gateVars);}
<gates>\n

<gateSet>[x-zX-Z]?[0-9]+\.[0-9]+		{tokens->push_back(TFCToken(GATE_SET,yytext));}
<gateSet>[x-zX-Z]?[0-9]+		  			{tokens->push_back(TFCToken(GATE_SET,yytext));}
<gateSet>\)												{BEGIN(gateVars);}
<gateSet>\n

<gateVars>{NAME}/'						{tokens->push_back(TFCToken(GATE_INPUT_N,string(yytext)));}
<gateVars>{NAME} 						{tokens->push_back(TFCToken(GATE_INPUT,string(yytext)));}
<gateVars>\"												{BEGIN(gateQoute);}
<gateVars>'
<gateVars>,
<gateVars>\n											 	{BEGIN(gates);}

<gateQoute>[[:alnum:][:space:]]*   	{tokens->push_back(TFCToken(GATE_INPUT,string(yytext)));}
<gateQoute>\"											 	{BEGIN(gateVars);}

<*>{WHITESPACE}
<*>{COMMENT}

%%

#include <fstream>

using namespace std;

vector<TFCToken> * lexCircuit(string fileName){
	ifstream * file = new ifstream(fileName.c_str());
	if (!file->good ()) return NULL;

	yyFlexLexer* lexer = new yyFlexLexer(file);
	if (!lexer) return NULL;
	tokens->clear ();
	while(lexer->yylex() != 0){
		;
	}
	return tokens;
}
