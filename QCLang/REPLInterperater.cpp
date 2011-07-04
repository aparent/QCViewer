#include "REPLInterperater.h"
#include "../simulate.h"
#include "../utility.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

REPL_Interperater::REPL_VAR::REPL_VAR(REPL_VALUE a,int b){
	type = b;
	if (type == COMPLEX){
		value.COMPLEX = new complex<float_type>(*a.COMPLEX);
	} else if (type == KET){
		value.STATE = new State(*a.STATE);
	} else{
		value = a;
	}
}

void printTree(QCLParseNode * node);  //XXX
QCLParseNode *parseQCL(string input); //XXX

string REPL_Interperater::runLine(string in){
	QCLParseNode * input =  parseQCL(in);
	evalTerm a = eval(input);
	delete input;
	return "Test";
	/*  XXX
	if (a.error){
		cout << "An error occurred" <<endl;
		return ERROR;
	}
	switch (a.type){
		case INT:
			cout << a.value.INT << endl;
			break;
		case FLOAT:
			cout << a.value.FLOAT << endl;
			break;
		case COMPLEX:
			cout << *a.value.COMPLEX << endl;
			break;
		case KET:
			a.value.STATE->print();
			break;
		case WIREMAP:
			printIntBin(a.value.INT);
			cout << endl;
			break;
		case MESSAGE:
			if (a.value.MESSAGE == SHOW_STATE){
				cout << "I would update the state drawing now!" << endl;
			}
			break;
		default:
			cout << "Unrecognized Type" << endl;
	}
	return DEFAULT;
	*/
}

State *REPL_Interperater::computeKet(string in){
	QCLParseNode * input =  parseQCL(in);
	evalTerm a = eval(input);
	if (a.error || a.type != KET){
		cout << "A parse error occurred or input was not a Ket" <<endl;
		return NULL;
	}
	return a.value.STATE;
}

REPL_Interperater::evalTerm REPL_Interperater::eval(QCLParseNode * in){
	evalTerm ret;
	if (in == NULL){
		return NULL;
	}
	switch (in->type){
		case KET:
			return getKet(string(in->value));
		case ID:
			return getVar(string(in->value));
		case INT:
			ret.value.INT = atoi(in->value);
			ret.type = INT;
			return ret;
		case KVAR:
			return getVar(string(in->value));
		case WIREMAP:
			return evalWireMap(in);
		case COMPLEX:
			ret.value.COMPLEX = new complex<float_type>(0,atof(in->value));
			ret.type = COMPLEX;
			return ret;
		case FLOAT:
			ret.value.FLOAT = atof(in->value);
			ret.type = FLOAT;
			return ret;
		case MINUS:
			return applyBinOP(MINUS,eval(in->leaves[0]),eval(in->leaves[1]));
		case PLUS:
			return applyBinOP(PLUS,eval(in->leaves[0]),eval(in->leaves[1]));
		case TIMES:
			return applyBinOP(TIMES,eval(in->leaves[0]),eval(in->leaves[1]));
		case EQUALS:
			return applyEquals(in->leaves[0], eval(in->leaves[1]));
		case DIV:
			return applyBinOP(DIV,eval(in->leaves[0]),eval(in->leaves[1]));
		case EXPONENT:
			return applyBinOP(EXPONENT,eval(in->leaves[0]),eval(in->leaves[1]));
		case OPERATION:
			return applyOPERATION(in->leaves[0]);  //XXX Must be fixed when applyOPERATION is fixed (need to pass line map as well)
		case FUNC:
			return Run_FUNC(in->value,in->leaves[0]);
		case INPUTS:
		default:
			ret.error = true;
			return ret;
	}
}


void REPL_Interperater::setVar(evalTerm right,	string var){
	if (!right.error){
			varMap[var]=REPL_VAR(right.value,right.type);
	} else {
		cout << "ERROR UNKNOWN TYPE" << endl;
	}
}

REPL_Interperater::evalTerm REPL_Interperater::getVar(string var){
	evalTerm ret;
	if (varMap.find(var) != varMap.end()){
		if (varMap[var].type == KET) {
			ret.value.STATE = new State(*varMap[var].value.STATE);
		} else if(varMap[var].type == COMPLEX){
			*ret.value.COMPLEX = *varMap[var].value.COMPLEX;
		} else {
			ret.value = varMap[var].value;
		}
		ret.type = varMap[var].type;
	} else {
		cout << "ERROR: variable not declared." << endl;
		ret.error = true;
	}
	return ret;
}

REPL_Interperater::evalTerm REPL_Interperater::evalWireMap(QCLParseNode * in){
	index_t wire = 0;
	if (in->leaves[2] != NULL){
		wire = (evalWireMap(in->leaves[2])).value.INT;
	}
	index_t first = atoi(in->leaves[0]->value);
	if (in->leaves[1] != NULL){
		index_t second = atoi(in->leaves[1]->value);
		for (unsigned int i=first; i < second; i++){
			wire = SetRegister(wire,i);
		}
	} else {
		wire = SetRegister(wire,first);
	}
	evalTerm ret;
	ret.type = WIREMAP;
	ret.value.INT = wire;
	return ret;
}

REPL_Interperater::evalTerm REPL_Interperater::getKet(string value){
	evalTerm ret;
	index_t basis = 0;
	ret.type = KET;
	for (int i = value.size()-1, j = 0; i>=0; i--,j++){
		if      (value[i] == '1') {
			basis = SetRegister(basis,j);
		} else if (value[i] == '0') {
		} else    {
			cout << "ERROR BAD CHAR IN KET: " << value[i] << endl;
		}
	}
	ret.value.STATE = new State(1,basis);
	ret.value.STATE->dim = ipow(2,value.size());
	return ret;
}

REPL_Interperater::evalTerm REPL_Interperater::applyEquals(QCLParseNode * in,	evalTerm right){
	if (right.error || (in->type != ID && in->type != KVAR)){
		right.error = true;
	}
	else{
		setVar(right,string(in->value));
	}
	return right;
}

REPL_Interperater::evalTerm REPL_Interperater::applyBinOP(int OP,evalTerm left,evalTerm right){
	order(left,right);
	promote(left,right);
	if (left.error || right.error) return left;
	switch (left.type){
		case KET:
			switch (right.type){
				case KET:
					switch (OP){
						case PLUS:
							*left.value.STATE += *right.value.STATE;
							delete right.value.STATE;
							return left;
						case MINUS:
							*left.value.STATE -= *right.value.STATE;
							delete right.value.STATE;
							return left;
						case TIMES:
							*left.value.STATE = kron(*left.value.STATE,*right.value.STATE);
							delete right.value.STATE;
							return left;
					}
					break;
				case COMPLEX:
					switch (OP){
						case TIMES:
							*left.value.STATE *= *right.value.COMPLEX;
							delete right.value.COMPLEX;
							return left;
					}
					break;
				case FLOAT:
					switch (OP){
						case TIMES:
							*left.value.STATE *= complex<float_type>(right.value.FLOAT);
							return left;
					}
				case INT:
					switch (OP){
						case TIMES:
							*left.value.STATE *= complex<float_type>(right.value.INT);
							return left;
						case EXPONENT:
							State temp = *left.value.STATE;
							for(unsigned int i = 0; i < right.value.INT-1; i++){
								*left.value.STATE = kron(*left.value.STATE,temp);
							}
							return left;
					}
					break;
			}
			break;
		case COMPLEX:
			switch (right.type){
				case COMPLEX:
					switch (OP){
						case PLUS:
							*left.value.COMPLEX+=*right.value.COMPLEX;
							delete right.value.COMPLEX;
							return left;
						case MINUS:
							*left.value.COMPLEX-=*right.value.COMPLEX;
							delete right.value.COMPLEX;
							return left;
						case TIMES:
							*left.value.COMPLEX*=*right.value.COMPLEX;
							delete right.value.COMPLEX;
							return left;
						case DIV:
							*left.value.COMPLEX/=*right.value.COMPLEX;
							delete right.value.COMPLEX;
							return left;
					}
					break;
			}
			break;
		case FLOAT:
			switch (right.type){
				case FLOAT:
					switch (OP){
						case PLUS:
							left.value.FLOAT+=right.value.FLOAT;
							return left;
						case MINUS:
							left.value.FLOAT-=right.value.FLOAT;
							return left;
						case TIMES:
							left.value.FLOAT*=right.value.FLOAT;
							return left;
						case DIV:
							left.value.FLOAT/=right.value.FLOAT;
							return left;
					}
			}
		case INT:
			switch (right.type){
				case INT:
					switch (OP){
						case PLUS:
							left.value.INT+=right.value.INT;
							return left;
						case MINUS:
							left.value.FLOAT-=right.value.INT;
							return left;
						case TIMES:
							left.value.FLOAT*=right.value.INT;
							return left;
						case DIV:
							left.value.FLOAT/=right.value.INT;
							return left;
						case EXPONENT:
							left.value.INT=ipow(left.value.INT,right.value.INT);
							return left;
					}
					break;
			}
			break;
	}
	cout << "Unsupported types in operation" << endl;
	evalTerm ret;
	ret.error = true;
	return ret;
}

REPL_Interperater::evalTerm REPL_Interperater::applyExponent(evalTerm left,evalTerm right){
	if (left.error || right.error ) return left;
	if (left.type == INT && right.type == INT ){
		left.value.INT=ipow(left.value.INT,right.value.INT);
		return left;
	}
	left.error =true;
	return left;
}

void REPL_Interperater::order(evalTerm &a,evalTerm &b){
	evalTerm t;
	if ( a.type == INT && b.type != INT){
		t = a;
		a = b;
		b = t;
	} else if ( a.type == FLOAT && b.type != FLOAT && b.type != INT){
		t = a;
		a = b;
		b = t;
	} else if ( a.type == COMPLEX && b.type != COMPLEX && b.type != FLOAT && b.type != INT){
		t = a;
		a = b;
		b = t;
	}
}

void REPL_Interperater::promote(evalTerm &a,evalTerm &b){//Use after ordering
	evalTerm t;
	if (a.type == FLOAT && b.type == INT){
		b.type = FLOAT;
		b.value.FLOAT = b.value.INT;
	} else if (a.type == COMPLEX && b.type == INT){
		b.type = COMPLEX;
		b.value.COMPLEX = new complex<float_type>(b.value.INT,0);
	}	else if (a.type == COMPLEX && b.type == FLOAT){
		b.type = COMPLEX;
		b.value.COMPLEX = new complex<float_type>(b.value.FLOAT,0);
	}
}

REPL_Interperater::evalTerm REPL_Interperater::Run_FUNC(string name , QCLParseNode * input){
	evalTerm ret;
	if (name.compare("setState")==0){
		evalTerm in =eval(input->leaves[0]);
		if (in.type == KET){
			if (Sim_State != NULL) delete Sim_State;
			Sim_State = in.value.STATE;
			ret.type = KET;
			ret.value.STATE = Sim_State;
			return ret;
		}
	} else if (name.compare("printState")==0){
		if (Sim_State != NULL){
			cout << "Printing from the print function!" << endl;
			Sim_State->print();
		}	else {
			cout << "ERROR: State not set" << endl;
		}
		ret.type = KET;
		ret.value.STATE = Sim_State;
		return ret;
	} else if (name.compare("showState")==0){
		if (Sim_State != NULL){
			ret.type = MESSAGE;
			ret.value.MESSAGE=SHOW_STATE;
			return ret;
		}	else {
			cout << "ERROR: State not set" << endl;
			return evalTerm(false);
		}
	}
	//No Function accepted ERROR
	cout << "ERROR: Function does not exist" << endl;
	return evalTerm(false);
}

//TODO: Make this only apply to the mapped lines as originally intended
//currently it just applies the op to the
REPL_Interperater::evalTerm REPL_Interperater::applyOPERATION(QCLParseNode * input){
	QCLParseNode * ops = input;
	while (ops->leaves[0]!= NULL){
		ops = ops->leaves[0];
		string name = ops->value;
		if (circMap.find(name) != circMap.end()){
			Circuit * c = circMap[name];
			if (ops->type == OPEXPONENT){
				evalTerm exp = eval(ops->leaves[1]);
				if (exp.type != INT){
					cout << "ERROR: Invalid Exponent type" << endl;
					return evalTerm(false);
				}
				for(unsigned int i = 0; i < exp.value.INT; i++){
					*Sim_State = ApplyCircuit (Sim_State,c);
				}
			} else {
				*Sim_State = ApplyCircuit (Sim_State,c);
			}
		}else{
			cout << "ERROR: Circuit does not exist" << endl;
			return evalTerm(false);
		}
	}
	evalTerm ret;
	ret.type = KET;
	ret.value.STATE = Sim_State;
	return ret;
}
