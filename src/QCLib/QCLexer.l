%option outfile="QCLexer.cpp"
%option c++
%option noyywrap
%option noyyalloc
%option noyyrealloc
%option noyyfree
%option never-interactive
%option unistd
%{
	#include "QCLexer.h"
	#include "utility.h"
	#include <iostream>
	vector<QCToken> *tokens = new vector<QCToken>;
%}

%x lineNames
%x lineQoute
%x gates
%x gateVars
%x gateQoute
%x gateSet
%x circ
%x circIn
%x loopcount

WHITESPACE [[:blank:]\t]+
COMMENT #.*
NAME [[:alnum:]]+

%%
\.v		tokens->push_back(QCToken(SEC_START,"V"));BEGIN(lineNames);
\.i		tokens->push_back(QCToken(SEC_START,"I"));BEGIN(lineNames);
\.o		tokens->push_back(QCToken(SEC_START,"O"));BEGIN(lineNames);
\.c		tokens->push_back(QCToken(SEC_START,"C"));BEGIN(lineNames);
\.(ol)		tokens->push_back(QCToken(SEC_START,"OL"));BEGIN(lineNames);
(?i:begin)	tokens->push_back(QCToken(SEC_START,"GATES"));BEGIN(circ);
\n

<lineNames>{NAME}			tokens->push_back(QCToken(VAR_NAME,yytext));
<lineNames>\"				BEGIN(lineQoute);
<lineNames>,
<lineNames>\n				BEGIN(INITIAL);
<lineQoute>[[:alnum:][:space:]]*	tokens->push_back(QCToken(VAR_NAME,yytext));
<lineQoute>\"				BEGIN(lineNames);

<circ>{NAME}				tokens->push_back(QCToken(CIRC_NAME,sToUpper(yytext)));BEGIN(circIn);
<circ>\n				BEGIN(gates);

<circIn>{NAME}			tokens->push_back(QCToken(VAR_NAME,yytext));
<circIn>\n			tokens->push_back(QCToken(NEWLINE,yytext));BEGIN(gates);


<gates>(?i:end)	tokens->push_back(QCToken(SEC_END,"GATES")); BEGIN(INITIAL);
<gates>{NAME}	tokens->push_back(QCToken(VAR_NAME,sToUpper(yytext)));
<gates>"^"	tokens->push_back(QCToken(EXPONENT,yytext));BEGIN(loopcount);
<gates>\(	BEGIN(gateSet);
<gates>[ ]	BEGIN(gateVars);
<gates>\n

<gateSet>[x-zX-Z]?[0-9]+\.[0-9]+	tokens->push_back(QCToken(GATE_SET,yytext));
<gateSet>[x-zX-Z]?[0-9]+		tokens->push_back(QCToken(GATE_SET,yytext));
<gateSet>\)				BEGIN(gateVars);
<gateSet>"^"				tokens->push_back(QCToken(EXPONENT,yytext));BEGIN(loopcount);
<gateSet>\n

<gateVars>{NAME}/'	tokens->push_back(QCToken(GATE_INPUT_N,string(yytext)));
<gateVars>{NAME}	tokens->push_back(QCToken(GATE_INPUT,string(yytext)));
<gateVars>\"		BEGIN(gateQoute);
<gateVars>'
<gateVars>,
<gateVars>\n		BEGIN(gates);

<loopcount>[0-9]+			tokens->push_back(QCToken(NUM,yytext));BEGIN(gateVars);

<gateQoute>[[:alnum:][:space:]]*	tokens->push_back(QCToken(GATE_INPUT,string(yytext)));
<gateQoute>\"				BEGIN(gateVars);

<*>{WHITESPACE}
<*>{COMMENT}

%%

#include <fstream>

using namespace std;

vector<QCToken> * lexCircuit(string fileName){
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
