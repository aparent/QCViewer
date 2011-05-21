%option outfile="TFCLexer.cpp"
%option c++
%option noyywrap
%{
	#include "TFCLexer.h"
	vector<TFCToken> *tokens = new vector<TFCToken>;
	int gate = 0;
%}

%x lineNames
%x gates
%x gateVars

%%
\.v           					{tokens->push_back(TFCToken(SEC_START,"V"));     BEGIN(lineNames);}
\.i           					{tokens->push_back(TFCToken(SEC_START,"I"));     BEGIN(lineNames);}
\.o           					{tokens->push_back(TFCToken(SEC_START,"O"));     BEGIN(lineNames);}
\.c          					  {tokens->push_back(TFCToken(SEC_START,"C"));     BEGIN(lineNames);}
\.(ol)									{tokens->push_back(TFCToken(SEC_START,"OL"));    BEGIN(lineNames);}
(?i:begin)							{tokens->push_back(TFCToken(SEC_START,"GATES")); BEGIN(gates);    }

<lineNames>\"[[:alnum:][:space:]]*\"|[[:alnum:]]+            {tokens->push_back(TFCToken(VAR_NAME,string(yytext)));}
<lineNames>,
<lineNames>\n						{BEGIN(INITIAL);}

<gates>(?i:end)         {tokens->push_back(TFCToken(SEC_END,"GATES")); BEGIN(INITIAL); }
<gates>[[:alnum:]]+ 		{tokens->push_back(TFCToken(VAR_NAME,yytext)); BEGIN(gateVars);}

<gateVars>\"[[:alnum:][:space:]]*\"|[[:alnum:]]+  {tokens->push_back(TFCToken(VAR_VALUE,yytext));}
<gateVars>,
<gateVars>\n									{BEGIN(gates);}

<*>[ \t\n]+							/* eat whitespace */
<*>#.*									/* eat whitespace */

%%

#include <fstream>

using namespace std;

vector<TFCToken> * lexCircuit(string fileName){
	ifstream * file = new ifstream(fileName.c_str());

	yyFlexLexer* lexer = new yyFlexLexer(file);
	while(lexer->yylex() != 0){
		;
	}
	return tokens;
}
