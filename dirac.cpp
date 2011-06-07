#include "dirac.h"
#include <cmath>
#include <string.h>
#include "armadillo/include/armadillo"
#include "diracParser.h"
#include "parseNode.h"
#include <iostream>//for error messages

using namespace std;
using namespace arma;

struct diracTerm{
	int type;
	cx_mat vecValue;
	cx_double numValue;
};

parseNode *parseDirac(std::string input); //Defined in diracParser.y
string printTree(parseNode *node);
diracTerm evalTree(parseNode *node);


stateVec *getStateVec (std::string input, bool normalize){
	parseNode *node = parseDirac(input); 
	if(node!=NULL){
		diracTerm term = evalTree(node);
		if (normalize){
			term.vecValue = term.vecValue/sqrt(cdot(term.vecValue,term.vecValue));
		}
		stateVec *result= new stateVec;
		cx_mat ket = term.vecValue;
		result->dim = ket.n_rows;
		result->data = new complex<float>[ket.n_rows];
		for(unsigned int i=0;i<ket.n_rows;i++){
			result->data[i] = ket(i,0);
		}
		return result;
	}
	else{
		cout << "PARSE ERROR" << endl;
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
	ret.type = KET;
  cx_mat zero(2,1); zero(0,0)=1; zero(1,0)=0;//|0>
  cx_mat one(2,1) ; one(0,0) =0; one(1,0) =1;//|1>
	if      (value[0] == '0') ret.vecValue = zero;
	else if (value[0] == '1') ret.vecValue = one;
	else    cout << "ERROR BAD CHAR IN KET: " << value[0] << endl;
	for(unsigned int i = 1;i<value.size();i++){
		if      (value[i] == '0') ret.vecValue = kron(ret.vecValue,zero);
		else if (value[i] == '1') ret.vecValue = kron(ret.vecValue,one);
		else    cout << "ERROR BAD CHAR IN KET: " << value[i] << endl;
	}
	return ret;
}

diracTerm constAdd(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.numValue = a.numValue + b.numValue;
	ret.type = NUM;
	return ret;
}

diracTerm constSub(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.numValue = a.numValue - b.numValue;
	ret.type = NUM;
	return ret;
}

diracTerm constMult(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.numValue = a.numValue * b.numValue;
	ret.type = NUM;
	return ret;
}

diracTerm ketAdd(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.vecValue = a.vecValue + b.vecValue;
	ret.type = KET;
	return ret;
}

diracTerm ketSub(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.vecValue = a.vecValue - b.vecValue;
	ret.type = KET;
	return ret;
}

diracTerm ketKron(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.vecValue = kron(a.vecValue,b.vecValue);
	ret.type = KET;
	return ret;
}

diracTerm constKetMult(diracTerm a, diracTerm b){
	diracTerm ret;
	ret.vecValue = a.numValue*b.vecValue;
	ret.type = KET;
	return ret;
}

diracTerm evalTree(parseNode *node){
	diracTerm ret;
	if(node->type == CNUM){
		ret.type = NUM;
		ret.numValue = cx_double(0,atof((node->value).c_str()));
		return ret;
	}
	if(node->type == NUM){
		ret.type = NUM;
		ret.numValue = cx_double(atof((node->value).c_str()),0);
		return ret;
	}
	if(node->type == KET){
		return stringToKet(node->value);
	}

	diracTerm left;
	diracTerm right;

	if(node->right == NULL){
		cout << "ERROR right is NULL."<<endl;
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
			cout << "ERROR cannot subtract vector and a number"<<endl;
			return ret;
		}
	}

	if(node->right == NULL){
		cout << "ERROR left is NULL."<<endl;
		return ret;
	}
	else left = evalTree(node->left);

	if (node->type == PLUS){
		if      (left.type == NUM && right.type == NUM) return constAdd(left,right);
		else if (left.type == KET && right.type == KET) return ketAdd(left,right);
		else{
			cout << "ERROR cannot add vector and a number"<<endl;
			return ret;
		}
	}
	if (node->type == MINUS){
		if      (left.type == NUM && right.type == NUM) return constSub(left,right);
		else if (left.type == KET && right.type == KET) return ketSub(left,right);
		else{
			cout << "ERROR cannot subtract vector and a number"<<endl;
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
			cout << "ERROR cannot divide a vector"<< endl;
			return ret;
		}
	}
	else{
			cout << "ERROR unrecognized type: "<< node->type << endl;
			return ret;
	}
}
