#ifndef QCL_PARSE_UTILS
#define QCL_PARSE_UTILS

struct QCLParseNode{
	int type;
	char * value;
	QCLParseNode ** leaves;
};

#include "QCLParser.h"

QCLParseNode * setupFOR(QCLParseNode * exp1, QCLParseNode * exp2, QCLParseNode * repBlock, QCLParseNode * nextBlock){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = FOR;
	ret->leaves = new QCLParseNode*[4];
	ret->leaves[0]=exp1;
	ret->leaves[1]=exp2;
	ret->leaves[2]=repBlock;
	ret->leaves[4]=nextBlock;
	return ret;
}

QCLParseNode * setupLINE(QCLParseNode * currentLine, QCLParseNode * nextLine){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = LINE;
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

QCLParseNode * setupOPERATION(QCLParseNode * ops, char * targetVar){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = OPERATION;
	ret->leaves = new QCLParseNode*[2];
	ret->leaves[0]=ops;
	ret->leaves[1]=setupVAR(KVAR,targetVar);
	return ret;
}

QCLParseNode * setupOP(char * name, QCLParseNode * next){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = OP;
	ret->value = name;
	ret->leaves = new QCLParseNode*[1];
	ret->leaves[0]=next;
	return ret;
}

QCLParseNode * setupOPEXPONENT(QCLParseNode * op, QCLParseNode * exp, QCLParseNode * next){
	QCLParseNode * ret = new QCLParseNode;
	ret->type = OPEXPONENT;
	ret->leaves = new QCLParseNode*[3];
	ret->leaves[0]=op;
	ret->leaves[1]=exp;
	ret->leaves[2]=next;
	return ret;
}

#endif
