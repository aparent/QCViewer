#include "../gate.h"

#include "utility.h"
#include <cmath>
#include <complex>

#ifndef M_PI
#define M_PI 3.14159265
#endif

RGate::RGate(float_type n_rot, Axis a) : rot(n_rot) {
  drawType = DEFAULT;
  type = RGATE;
  axis = a;
}

Gate* RGate::clone(){
	RGate* g = new RGate(rot,axis);
	g->controls = controls;
	g->targets = targets;
	return g;
}

string RGate::getName(){
  return "R";
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
  switch (axis) {
    case X:
      {
        float_type cosr = cos (M_PI*rot/2.0);
        float_type sinr = sin (M_PI*rot/2.0);
        if (GetRegister (bits, targets.at (0))) {
          *answer = State (complex<float_type>(sinr,0),BuildBitString(bits,0));
          *answer += State (complex<float_type>(cosr,0),BuildBitString(bits,1));
        } else {
          *answer = State (complex<float_type>(-cosr,0),BuildBitString(bits,0));
          *answer += State (complex<float_type>(sinr,0),BuildBitString(bits,1));
        }
      }
      break;
    case Y:
      {
        float_type cosr = cos (M_PI*rot/2.0);
        float_type sinr = sin (M_PI*rot/2.0);
        if (GetRegister (bits, targets.at (0))) {
          *answer = State (complex<float_type>(0,-sinr),BuildBitString(bits,0));
          *answer += State (complex<float_type>(cosr,0),BuildBitString(bits,1));
        } else {
          *answer = State (complex<float_type>(-cosr,0),BuildBitString(bits,0));
          *answer += State (complex<float_type>(0,sinr),BuildBitString(bits,1));
        }
      }
      break;
    case Z:
      {
        if (GetRegister (bits, targets.at(0))){
          *answer = State(exp(complex<float_type>(0,M_PI*rot/2.0)), bits);// XXX: verify do not need buildbitstring
        }
        else{
          *answer = State(1, bits);
        }
      }
     break;
  }
  return answer;
}


index_t RGate::BuildBitString (index_t orig, unsigned int ans) {
  unsigned int output = orig;
  if (ans) {
    output = SetRegister (output, targets.at(0));
  }  else {
    output = UnsetRegister (output, targets.at(0));
  }
  return output;
}

void RGate::set_axis (Axis a) {
  axis = a;
}

RGate::Axis RGate::get_axis () {
  return axis;
}

void RGate::set_rotVal (float_type r) { // XXX: remove float_type, consildate this stuff!!
  rot = r;
}

float_type RGate::get_rotVal () {
  return rot;
}
