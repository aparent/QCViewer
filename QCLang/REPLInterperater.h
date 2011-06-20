#ifndef QCL_REPL_INTERPERATER
#define QCL_REPL_INTERPERATER

#include "QCLParserUtils.h"
#include <map>
#include <string>
#include <complex>
#include "../state.h"

//enum int{INT,COMPLEX,FLOAT,STATE}; 
union REPL_VALUE{
		index_t INT;
		std::complex<float_type>* COMPLEX;
		float_type FLOAT;
		State * STATE;
};

struct REPL_VAR{
	REPL_VAR(REPL_VALUE a,int b) : value(a),type(b){}
	REPL_VAR(){}
	REPL_VALUE value;
	int type;
};

struct evalTerm{
	evalTerm() : error(false){}
	evalTerm(REPL_VALUE a,int b) : value(a),type(b){}
	bool error;
	REPL_VALUE value;
	int type;
};

struct REPL_Interperater{
		std::map<std::string,REPL_VAR> varMap;
		void runLine(std::string);
		evalTerm eval(QCLParseNode *);
		State *computeKet(std::string);

		void setVar(evalTerm,	std::string);
		evalTerm getVar(std::string);
		evalTerm getKet(std::string);

		evalTerm applyEquals(QCLParseNode*,evalTerm);
		evalTerm evalWireMap(QCLParseNode*);

		evalTerm applyBinOP(int,evalTerm,evalTerm);
		evalTerm applyExponent(evalTerm left,evalTerm right);

		void order(evalTerm &a,evalTerm &b);
		void promote(evalTerm &a,evalTerm &b);
};

#endif
