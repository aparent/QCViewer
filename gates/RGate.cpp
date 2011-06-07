#include "../gate.h"

#include "utility.h"
#include <cmath>
#include <complex>
#define PI 3.14159265

RGate::RGate(float_t n_rot) : rot(n_rot) {
	drawType = DEFAULT;
	type = RGATE;
}

string RGate::getName(){
	stringstream ss;
	ss << "R(" << rot << ")";
	return ss.str();
}

/* RGate simulation implimentation */
State *RGate::applyToBasis(index_t bitString){
  // First, make sure all of the controls are satisfied.
  bool ctrl = true;
  for (unsigned int i = 0; i < controls.size(); i++) {
    Control c = controls[i];
    int check = GetRegister (bitString, c.wire);
    if (!c.polarity != check) {
      ctrl = false; // control line not satisfied.
      break;
    }
  }
  if (ctrl) {
    return ApplyU (bitString);
  } else {
    State *answer = new State (1, bitString); // with amplitude 1 the input bitString is unchanged
    return answer;
	}
}

State *RGate::ApplyU (index_t bits){
  State *answer = new State;
	if (GetRegister (bits, targets.at(0))){
		*answer = State(exp(complex<float_t>(0,PI*rot)), BuildBitString (bits, 1));
	}
	else{
		*answer = State(1, bits);
	}
  return answer;
}


index_t RGate::BuildBitString (index_t orig, unsigned int ans) {
  unsigned int output = orig;
  if (ans) {
		output = SetRegister (output, targets.at(0));
  }
  return output;
}
