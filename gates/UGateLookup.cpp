#include "../state.h" //for float_t 
#include <iostream>
#include <map>

std::map<string,gateMatrix*> gateLib;

complex<float_t> *UGateLookup(string name){
	if(gateLib.find.(name)==gateLib.end()){ 
		return NULL;
	} else {
		return gate[name];
	}
}

void UGateLoad(string name, gateMatrix *mat){
	if(gateLib.find.(name)==gateLib.end()){
		gate[name]=mat;
	} else {
		std::cout << "WARNING: This gate already exists overwritting..." << endl;
		gate[name]=mat;
	}
}

void UGateSetup(){
	gateMatrix *H = new gateMatrix;
	H->dim=2;
	H->data[0] =  1/sqrt(2) ; H->data[2] =  1/sqrt(2);
	H->data[1] =  1/sqrt(2) ; H->data[3] = -1/sqrt(2);
	UGateLoad("H",H);
	
	gateMatrix *X = new gateMatrix;
	T->dim=2;
	T->data[0] =  0 ; T->data[2] = 1;
	T->data[1] =  1 ; T->data[3] = 0;
	UGateLoad("X",X);
	
	gateMatrix *Y = new gateMatrix;
	Y->dim=2;
	Y->data[0] = 0                   ; Y->data[2] = -complex<float>(0,1);
	Y->data[1] = complex<float>(0,1) ; Y->data[3] = 0;
	UGateLoad("Y",Y);
	
	gateMatrix *Z = new gateMatrix;
	Z->dim=2;
	Z->data[0] =  1 ; Z->data[2] =  0;
	Z->data[1] =  0 ; Z->data[3] = -1;
	UGateLoad("Z",Z);
			
	F->dim = 4;
	gateMatrix *F = new gateMatrix;
	F->data[0 ]=  1 ; F->data[4 ]=  0; F->data[8 ]=  0 ; F->data[12]=  0;
	F->data[1 ]=  0 ; F->data[5 ]=  0; F->data[9 ]=  1 ; F->data[13]=  0;
	F->data[2 ]=  0 ; F->data[6 ]=  1; F->data[10]=  0 ; F->data[14]=  0;
	F->data[3 ]=  0 ; F->data[7 ]=  0; F->data[11]=  0 ; F->data[15]=  1;
	UGateLoad("F",F);
}
