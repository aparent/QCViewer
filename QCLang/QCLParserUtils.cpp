#include "QCLParserUtils.h"
#include <stdio.h>
#include <string.h>

QCLParseNode * setupFOR(QCLParseNode * exp1, QCLParseNode * exp2, QCLParseNode * repBlock, QCLParseNode * nextBlock){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = FOR;
	ret->leaves = new QCLParseNode*[4];
	ret->leaves[0]=exp1;
	ret->leaves[1]=exp2;
	ret->leaves[2]=repBlock;
	ret->leaves[3]=nextBlock;
	return ret;
}

QCLParseNode * setupLINE(QCLParseNode * currentLine, QCLParseNode * nextLine){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = LINE;
	ret->value = NULL;
	ret->leaves = new QCLParseNode*[2];
	ret->leaves[0]=currentLine;
	ret->leaves[1]=nextLine;
	return ret;
}

QCLParseNode * setupBINOP(int type, QCLParseNode * arg1, QCLParseNode * arg2){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = type;
	ret->leaves = new QCLParseNode*[2];
	ret->leaves[0]=arg1;
	ret->leaves[1]=arg2;
	return ret;
}

QCLParseNode * setupVAR(int type, char* name){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = type;
	ret->value = name;
	ret->leaves = NULL;
	return ret;
}

QCLParseNode * setupOPERATION(QCLParseNode * ops, QCLParseNode * targetVar){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = OPERATION;
	ret->leaves = new QCLParseNode*[2];
	ret->leaves[0]=ops;
	ret->leaves[1]=targetVar;
	return ret;
}

QCLParseNode * setupOP(QCLParseNode * op, QCLParseNode * next){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = OP;
	ret->value = strdup(op->value);
	delete op;
	ret->leaves = new QCLParseNode*[1];
	ret->leaves[0]=next;
	return ret;
}

QCLParseNode * setupOPEXPONENT(QCLParseNode * op, QCLParseNode * exp, QCLParseNode * next){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = OPEXPONENT;
	ret->value = strdup(op->value);
	delete op;
	ret->leaves = new QCLParseNode*[3];
	ret->leaves[0]=exp;
	ret->leaves[1]=next;
	return ret;
}

QCLParseNode * setupFUNC(QCLParseNode * id, QCLParseNode * inputs){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = FUNC;
	ret->value = strdup(id->value);
	delete id;
	ret->leaves = new QCLParseNode*[1];
	ret->leaves[0] = inputs;
	return ret;
}

QCLParseNode * setupINPUTS(QCLParseNode * input, QCLParseNode * next){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = INPUTS;
	ret->leaves = new QCLParseNode*[2];
	ret->leaves[0]=input;
	ret->leaves[1]=next;
	return ret;
}

std::string QCLNodeName(int type){
	switch(type){
		case KET:
			return "KET";
		case ID:
			return "ID";
		case NUM:
			return "NUM";
		case KVAR:
			return "KVAR";
		case OP:
			return "OP";
		case MINUS:
			return "MINUS";		
		case PLUS:
			return "PLUS";		
		case TIMES:
			return "TIMES";		
		case EQUALS:
			return "EQUALS";		
		case DIV:
			return "DIV";		
		case EXPONENT:
			return "EXPONENT";		
		case FOR:
			return "FOR";		
		case LINE:
			return "LINE";		
		case OPERATION:
			return "OPERATION";		
		case OPEXPONENT:
			return "OPEXPONENT";		
		case FUNC:
			return "FUNC";		
		case INPUTS:
			return "INPUTS";		
		default:
			return "??";
	}
}
int QCLNodeNumLeaves(int type){
	switch(type){
		case KET:
			return 0;
		case ID:
			return 0;
		case NUM:
			return 0;
		case KVAR:
			return 0;
		case OP:
			return 1;
		case MINUS:
			return 2;		
		case PLUS:
			return 2;		
		case TIMES:
			return 2;		
		case EQUALS:
			return 2;		
		case DIV:
			return 2;		
		case EXPONENT:
			return 2;		
		case FOR:
			return 4;		
		case LINE:
			return 2;		
		case OPERATION:
			return 1;		
		case OPEXPONENT:
			return 2;		
		case FUNC:
			return 1;		
		case INPUTS:
			return 2;		
		default:
			return -1;
	}
}

