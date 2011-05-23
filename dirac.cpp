#include "dirac.h"

string printTree(parseNode *node){
	if (node->type==SQRT) return node->value + "(" + printTree(node->right)+")";
	if (node->left == NULL && node->right == NULL && node->type==KET ) return "|" + node->value+">";
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
	for(int i = 1;i<value.size();i++){
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

	diracTerm right = evalTree(node->right);

	if (node->type == SQRT){
		if (right.type == NUM){
			ret.numValue=sqrt(right.numValue);
			ret.type = NUM;
			return ret;
		}
 		else{
			cout << "ERROR cannot subtract vector and a number";
			return ret;
		}
	}

	diracTerm left = evalTree(node->left);

	if (node->type == PLUS){
		if      (left.type == NUM && right.type == NUM) return constAdd(left,right);
		else if (left.type == KET && right.type == KET) return ketAdd(left,right);
		else{
			cout << "ERROR cannot add vector and a number";
			return ret;
		}
	}
	if (node->type == MINUS){
		if      (left.type == NUM && right.type == NUM) return constSub(left,right);
		else if (left.type == KET && right.type == KET) return ketSub(left,right);
		else{
			cout << "ERROR cannot subtract vector and a number";
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
			cout << "ERROR cannot divide a vector";
			return ret;
		}
	}
}
