#ifndef QCL_REPL_INTERPERATER
#define QCL_REPL_INTERPERATER

#include "QCLParserUtils.h"
#include <map>
#include <string>
#include <complex>
#include "../circuit.h"
#include "../state.h"


class REPL_Interperater{
	public:
		REPL_Interperater(): Sim_State(NULL){}
		enum message {DEFAULT,ERROR,SHOW_STATE};
		//Union of possible values for a variable 
		union REPL_VALUE{
			index_t INT;
			message MESSAGE;
			std::complex<float_type>* COMPLEX;
			float_type FLOAT;
			State * STATE;
		};
		//Evaluation term, includes information about the success of the operation. 
		struct evalTerm{
			evalTerm() :error(false),type(-1){}
			evalTerm(REPL_VALUE a,int b) : error(false), value(a),type(b){}
			evalTerm(bool a) : error(a),type(-1){}
			bool error;
			REPL_VALUE value;
			int type;
		};
		string runLine(std::string);
		State *computeKet(std::string);
		State *Sim_State;
		std::map<std::string,Circuit*> circMap;
	private:
		
		//Interpreter variable
		struct REPL_VAR{
			REPL_VAR(REPL_VALUE a,int b);
			REPL_VAR():type(-1){}
			REPL_VALUE value;
			int type;
		};		
		evalTerm eval(QCLParseNode *);
		evalTerm Run_FUNC(std::string, QCLParseNode *);
		evalTerm applyOPERATION(QCLParseNode * input);

		void setVar(evalTerm,	std::string);
		evalTerm getVar(std::string);
		evalTerm getKet(std::string);

		std::map<std::string,REPL_VAR> varMap;

		evalTerm applyEquals(QCLParseNode*,evalTerm);
		evalTerm evalWireMap(QCLParseNode*);

		evalTerm applyBinOP(int,evalTerm,evalTerm);
		evalTerm applyExponent(evalTerm left,evalTerm right);

		void order(evalTerm &a,evalTerm &b);
		void promote(evalTerm &a,evalTerm &b);
};

#endif
