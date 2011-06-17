#include "REPLInterperater.h"
#include "../utility.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

void printTree(QCLParseNode * node);
QCLParseNode *parseQCL(string input);

void REPL_Interperater::runLine(string in){
	QCLParseNode * input =  parseQCL(in);
	evalTerm a = eval(input);
	if (a.error){
		cout << "An error occured" <<endl;
		return;
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
		default:
			cout << "Unrecognized Type" << endl;
	}
}

evalTerm REPL_Interperater::eval(QCLParseNode * in){
	evalTerm ret;
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
		case OP:
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
		case OPEXPONENT:
		case FUNC:
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

evalTerm REPL_Interperater::getVar(string var){
	evalTerm ret;
	if (varMap.find(var) != varMap.end()){
		ret.value = varMap[var].value;
		ret.type = varMap[var].type;
	} else {
		cout << "ERROR: varible not declared." << endl;
		ret.error = true;
	}
	return ret;
}

evalTerm REPL_Interperater::evalWireMap(QCLParseNode * in){
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

evalTerm REPL_Interperater::getKet(string value){
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

evalTerm REPL_Interperater::applyEquals(QCLParseNode * in,	evalTerm right){
	if (right.error || (in->type != ID && in->type != KVAR)){
		right.error = true;
	}
	else{
		setVar(right,string(in->value));
	}
	return right;
}

evalTerm REPL_Interperater::applyBinOP(int OP,evalTerm left,evalTerm right){
	order(left,right);
	if (left.error || right.error) return left;
	switch (left.type){
		case KET:
			switch (right.type){
				case KET:
					switch (OP){
						case PLUS:
							*left.value.STATE += *right.value.STATE;
							return left;
						case MINUS:
							*left.value.STATE -= *right.value.STATE;
							return left;
						case TIMES:
							*left.value.STATE = kron(*left.value.STATE,*right.value.STATE);
							return left;
					}
					break;
				case COMPLEX:
					switch (OP){
						case TIMES:
							*left.value.STATE *= *right.value.COMPLEX;
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
							return left;
						case MINUS:
							*left.value.COMPLEX-=*right.value.COMPLEX;
							return left;
						case TIMES:
							*left.value.COMPLEX*=*right.value.COMPLEX;
							return left;
						case DIV:
							*left.value.COMPLEX/=*right.value.COMPLEX;
							return left;
					}
					break;
				case FLOAT:
					switch (OP){
						case PLUS:
							*left.value.COMPLEX+=right.value.FLOAT;
							return left;
						case MINUS:
							*left.value.COMPLEX-=right.value.FLOAT;
							return left;
						case TIMES:
							*left.value.COMPLEX*=right.value.FLOAT;
							return left;
						case DIV:
							*left.value.COMPLEX/=right.value.FLOAT;
							return left;
					}
					break;
				case INT:
					switch (OP){
						case PLUS:
							*left.value.COMPLEX+=right.value.INT;
							return left;
						case MINUS:
							*left.value.COMPLEX-=right.value.INT;
							return left;
						case TIMES:
							*left.value.COMPLEX*=right.value.INT;
							return left;
						case DIV:
							*left.value.COMPLEX/=right.value.INT;
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
				case INT:
					switch (OP){
						case PLUS:
							left.value.FLOAT+=right.value.INT;
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
					}
					break;
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

evalTerm REPL_Interperater::applyExponent(evalTerm left,evalTerm right){
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
//evalTerm REPL_Interperater::applyMinus(evalTerm left,evalTerm right);
