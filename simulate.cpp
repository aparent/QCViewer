#include "simulate.h"
#include <cmath>
#include <string>
#include <iostream>//XXX
#include "utility.h"
#include "gates/UGateLookup.h"


gateMatrix getGateMatrix(Gate*);//defined below

/*
 Registers: stored as a bitstring, register i at bit i etc.
*/

/*
	This function takes an input quantum state and a gate.
	It splits the input superposition into basis states (in the computational basis,
	i.e. into class bit strings.) It runs the gate on each of these, and collates
	the answers (weighed appropriately) into the answer.
	NULL on error (couldn't find a unitary.).  
	The application of the gate is left up to the gate class.  This allows per gate class 
	optimization if nessicary. (For a good example of how this works see gates/UGate.cpp
*/
State ApplyGate (State* in, Gate* g) {
  map<index_t, complex<float_t> >::iterator it;
  State answer;
	answer.dim = in->dim;
  for (it = in->data.begin(); it != in->data.end(); it++) {
    State* tmp = g->applyToBasis(it->first);
    if (tmp == NULL){
			 return NULL;
		}
    complex<float_t> foo = (*it).second;
    *tmp *= foo;
    answer += *tmp;
    delete tmp;
  }
  return answer;
}
