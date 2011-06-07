#include "../gate.h"

RGate::RGate(float_t n_rot) : rot(n_rot) {
	type = RGATE;
}

string RGate::getName(){
	stringstream ss;
	ss << "R(" << rot << ")";
	return "R";
}

State *RGate::applyToBasis(index_t bitString){
	return NULL;
}
