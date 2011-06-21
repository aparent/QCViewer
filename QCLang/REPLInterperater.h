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

/*MyString& MyString::operator=(const MyString& cSource)
{
 // check for self-assignment
		    if (this == &cSource)
		        return *this;
		 
		    // first we need to deallocate any value that this string is holding!
		    delete[] m_pchString;
		 
		    // because m_nLength is not a pointer, we can shallow copy it
		    m_nLength = cSource.m_nLength;
		 
		    // now we need to deep copy m_pchString
		    if (cSource.m_pchString)
		    {
			        // allocate memory for our copy
			        m_pchString = new char[m_nLength];
			 
			        // Copy the parameter the newly allocated memory
			        strncpy(m_pchString, cSource.m_pchString, m_nLength);
			    }
			    else
			        m_pchString = 0;
			 
			    return *this;
}*/

struct REPL_VAR{
	REPL_VAR(REPL_VALUE a,int b);	
	REPL_VAR(){}
	REPL_VALUE value;
	int type;
};

struct evalTerm{
	evalTerm() : error(false){}
	evalTerm(REPL_VALUE a,int b) : value(a),type(b){}
	evalTerm(bool a) : error(a){}
	bool error;
	REPL_VALUE value;
	int type;
};

class REPL_Interperater{
	public:
		REPL_Interperater(): Sim_State(NULL){}
		void runLine(std::string);
		State *computeKet(std::string);
		State *Sim_State;
	private:
		
		evalTerm eval(QCLParseNode *);
		evalTerm Run_FUNC(std::string, QCLParseNode *);

		void setVar(evalTerm,	std::string);
		evalTerm getVar(std::string);
		evalTerm getKet(std::string);

		std::map<std::string,REPL_VAR> varMap;
		//std::map<std::string,Circuit*> circMap;

		evalTerm applyEquals(QCLParseNode*,evalTerm);
		evalTerm evalWireMap(QCLParseNode*);

		evalTerm applyBinOP(int,evalTerm,evalTerm);
		evalTerm applyExponent(evalTerm left,evalTerm right);

		void order(evalTerm &a,evalTerm &b);
		void promote(evalTerm &a,evalTerm &b);
};

#endif
