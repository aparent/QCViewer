#include "dirac.h"
#include "state.h"
#include "utility.h"
#include "diracParser.h"
#include "parseNode.h"
#include <iostream>//for error messages
#include <stdlib.h> //atof

using namespace std;

struct diracTerm{
	int type;
	State vecValue;
	complex<float_type> numValue;
};

parseNode *parseDirac(std::string input); //Defined in diracParser.y
string printTree(parseNode *node);
diracTerm evalTree(parseNode *node);


State *getStateVec (std::string input, bool normalize){
	parseNode *node = parseDirac(input);
	if(node!=NULL){
		diracTerm term = evalTree(node);
		if (term.type != KET ){
			return NULL;
		}
		if (normalize){
			term.vecValue.normalize();
		}
		State *ret = new State(term.vecValue);
		return ret;
	}
	else{
		cerr << "PARSE ERROR" << endl;
		return NULL;
	}
}

string printTree(parseNode *node){
	if (node->type==SQRT) 																							return node->value + "(" + printTree(node->right)+")";
	if (node->left == NULL && node->right == NULL && node->type==KET ) 	return "|" + node->value+">";
	if (node->left == NULL && node->right == NULL && node->type==CNUM ) return node->value+"i";
	if (node->left == NULL && node->right == NULL ) return node->value;
	return "("+printTree(node->left)+ " " +node->value + " " + printTree(node->right)+")";
}

diracTerm stringToKet(string value){
	diracTerm ret;
	index_t basis = 0;
	ret.type = KET;
	for (int i = value.size()-1, j = 0; i>=0; i--,j++){
		if      (value[i] == '1') {
			basis = SetRegister(basis,j);
		} else if (value[i] == '0') {
		} else    {
			cerr << "ERROR BAD CHAR IN KET: " << value[i] << endl;
		}
	}
	ret.vecValue = State(1,basis);
	ret.vecValue.dim = ipow(2,value.size());
	return ret;
}

diracTerm constAdd(diracTerm a, diracTerm b){
	a.numValue += b.numValue;
	return a;
}

diracTerm constSub(diracTerm a, diracTerm b){
	a.numValue += b.numValue;
	return a;
}

diracTerm constMult(diracTerm a, diracTerm b){
	a.numValue *= b.numValue;
	return a;
}

diracTerm ketAdd(diracTerm a, diracTerm b){
	a.vecValue += b.vecValue;
	return a;
}

diracTerm ketSub(diracTerm a, diracTerm b){
	a.vecValue -= b.vecValue;
	return a;
}

diracTerm ketKron(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.vecValue = kron(a.vecValue,b.vecValue);
	ret.type = KET;
	return ret;
}

diracTerm constKetMult(diracTerm a, diracTerm b){
	b.vecValue*=a.numValue;
	return b;
}

diracTerm evalTree(parseNode *node){
	diracTerm ret;
	if(node->type == CNUM){
		ret.type = NUM;
		ret.numValue = complex<float_type>(0,atof((node->value).c_str()));
		return ret;
	}
	if(node->type == NUM){
		ret.type = NUM;
		ret.numValue = complex<float_type>(atof((node->value).c_str()),0);
		return ret;
	}
	if(node->type == KET){
		return stringToKet(node->value);
	}

	diracTerm left;
	diracTerm right;

	if(node->right == NULL){
		cerr << "ERROR right is NULL."<<endl;
		return ret;
	}
	else right = evalTree(node->right);

	if (node->type == SQRT){
		if (right.type == NUM){
			ret.numValue=sqrt(right.numValue);
			ret.type = NUM;
			return ret;
		}
 		else{
			cerr << "ERROR cannot subtract vector and a number"<<endl;
			return ret;
		}
	}

	if(node->right == NULL){
		cerr << "ERROR left is NULL."<<endl;
		return ret;
	}
	else left = evalTree(node->left);

	if (node->type == PLUS){
		if      (left.type == NUM && right.type == NUM) return constAdd(left,right);
		else if (left.type == KET && right.type == KET) return ketAdd(left,right);
		else{
			cerr << "ERROR cannot add vector and a number"<<endl;
			return ret;
		}
	}
	if (node->type == MINUS){
		if      (left.type == NUM && right.type == NUM) return constSub(left,right);
		else if (left.type == KET && right.type == KET) return ketSub(left,right);
		else{
			cerr << "ERROR cannot subtract vector and a number"<<endl;
			return ret;
		}
	}
	if (node->type == TIMES){
		if      (left.type == NUM && right.type == NUM) return constMult(left,right);
		else if (left.type == KET && right.type == KET) return ketKron(left,right);
		else{
			if    (left.type == KET && right.type == NUM) return constKetMult(right,left);
			else                                          return constKetMult(left,right);
		}
	}
	if (node->type == DIV){
		if (left.type == NUM && right.type == NUM){
			ret.type = NUM;
			ret.numValue = left.numValue / right.numValue;
			return ret;
		}
		else{
			cerr << "ERROR cannot divide a vector"<< endl;
			return ret;
		}
	}
	else{
			cerr << "ERROR unrecognized type: "<< node->type << endl;
			return ret;
	}
}
