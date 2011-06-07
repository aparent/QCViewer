//Implimentation code for the UGate
#include "../gate.h"
#include "UGateLookup.h"
#include <iostream>
#include <utility.h>
using namespace std;

UGate::UGate(string n_name) : name(n_name) {
	type = UGATE;
}

string UGate::getName(){
	return name;
}
void UGate::setName(string n_name){
	name = n_name;
}
		
/*
	Applies the gate to a single basis state sim does this classically 
	for all basis states in the superposition.  Note this can return a
	superpostion.
*/
State *UGate::applyToBasis(index_t bitString){
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

// Takes care of actually applying the matrix by indexing to the correct matrix coloum
State *UGate::ApplyU (index_t bits){
	unsigned int input = ExtractInput (bits);
  // now, go through all rows of the output from the correct column of U
  State *answer = new State;
	if (matrix == NULL){
		matrix = UGateLookup(name);
	}
  for (unsigned int i = 0; i < matrix->dim; i++) {
    if (matrix->data[input*matrix->dim+i] != complex<float_t>(0)){
			*answer += State(matrix->data[input*matrix->dim+i], BuildBitString (bits, i));
		}
  }
  return answer;
}

/*
	This function takes an input bitstring and the mapping of targets and returns
	the input to the gate, for the purpose of indexing into its matrix.
*/
unsigned int UGate::ExtractInput (index_t bitString) {
  unsigned int input = 0;
  for (unsigned int i = 0; i < targets.size(); i++) {
    if (GetRegister (bitString, targets.at(i))) {
      input = SetRegister (input, i);
    }
  }
  return input;
}

/*
	This function takes an input string, the mapping of targets, and an output for those targets
	and produces the output bitstring
*/
index_t UGate::BuildBitString (index_t orig, unsigned int ans) {
  unsigned int output = orig;
  for (unsigned int i = 0; i < targets.size (); i++) {
    if (GetRegister (ans, i)) {
			output = SetRegister (output, targets.at(i));
    } else {
      output = UnsetRegister (output, targets.at(i));
    }
  }
  return output;
}
