#ifndef QCL_PARSE_UTILS
#define QCL_PARSE_UTILS

#include <string>

struct QCLParseNode{
	~QCLParseNode();
	QCLParseNode() : type(0),value(NULL),leaves(NULL) { }
	int type;
	char * value;
	QCLParseNode ** leaves;
};

#include "QCLParser.h"

QCLParseNode * setupWIREMAP(QCLParseNode*, QCLParseNode*, QCLParseNode*);
QCLParseNode * setupFOR(QCLParseNode*, QCLParseNode*, QCLParseNode*, QCLParseNode*);
QCLParseNode * setupLINE(QCLParseNode * currentLine, QCLParseNode * nextLine);
QCLParseNode * setupBINOP(int type, QCLParseNode * arg1, QCLParseNode * arg2);
QCLParseNode * setupVAR(int type, char* name);
QCLParseNode * setupOPERATION(QCLParseNode * ops, QCLParseNode * targetVar);
QCLParseNode * setupOP(QCLParseNode * op, QCLParseNode * next);
QCLParseNode * setupOPEXPONENT(QCLParseNode * op, QCLParseNode * exp, QCLParseNode * next);
QCLParseNode * setupFUNC(QCLParseNode * id, QCLParseNode * inputs);
QCLParseNode * setupINPUTS(QCLParseNode * input, QCLParseNode * next);
int QCLNodeNumLeaves(int type);
std::string QCLNodeName(int type);

#endif
