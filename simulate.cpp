#include "circuit.h"
#include "gate.h"
#include "state.h"
#include <string>

/*
 Registers: stored as a bitstring, register i at bit i etc.
*/
unsigned int SetRegister (unsigned int bits, unsigned int reg) {
  return bits | 1<<reg;
}

unsigned int UnsetRegister (unsigned int bits, unsigned int reg) {
  return bits & ~(1<<reg);
}

unsigned int GetRegister (unsigned int bits, unsigned int reg) {
  return (bits & 1 << reg) >> reg;
}

/* This function takes an input bitstring and the mapping of targets and returns
   the input to the gate, for the purpose of indexing into its matrix.
*/
unsigned int ExtractInput (index_t bitString, vector<int>* targetMap) {
  unsigned int input = 0;
  for (unsigned int i = 0; i < targetMap->size(); i++) {
    if (GetRegister (bitString, (*targetMap)[i])) {
      input = SetRegister (input, i);
    }
  }
  return input;
}

/* This function takes an input string, the mapping of targets, and an output for those targets
   and produces the output bitstring
*/
index_t BuildBitString (index_t orig, vector<int>* targetMap, unsigned int ans) {
  unsigned int output = orig;
  for (unsigned int i = 0; i < targetMap->size (); i++) {
    if (GetRegister (ans, i)) {
      output = SetRegister (output, (*targetMap)[i]);
    } else {
      output = UnsetRegister (output, (*targetMap)[i]);
    }
  }
  return output;
}

/* This function takes an entire input basis vector, the matrix to apply and
   the targets, which describe which bits map to which inputs of the matrix.
   It returns a (usually very sparse) quantum state.
*/
State* ApplyU (index_t bits, vector<int>* targets, complex<float_t>* matrix) {
  unsigned int input = ExtractInput (bits, targets);

  // now, go through all rows of the output from the correct column of U
  State *answer = new State;
  for (unsigned int i = 0; i < sqrt(sizeof(matrix)); i++) {
    *answer += State (matrix[input+i], i);
  }
  return answer;
}

/* This function takes a computational basis element and a gate
   and simulates. It returns a quantum state (usually very sparse.)
*/
State* ApplyGateToBasis (index_t bitString, Gate *g) {
  // First, make sure all of the controls are satisfied.
  bool ctrl = true;
  for (unsigned int i = 0; i < g->controls.size(); i++) {
    Control c = g->controls[i];
    int check = GetRegister (bitString, c.wire);
    if ((!c.polarity && !check) || (c.polarity && check)) {
      ctrl = false; // control line not satisfied.
      break;
    }
  }
  if (ctrl) {
    return ApplyU (bitString, &g->targets, g->matrix);
  } else {
    State *answer = new State (bitString, 1); // with amplitude 1 the input bitString is unchanged
    return answer;
  }
}

/* This function takes an input quantum state and a gate.
   It splits the input superposition into basis states (in the computational basis,
   i.e. into class bit strings.) It runs the gate on each of these, and collates
   the answers (weighed appropriately) into the answer.
   NULL on error (couldn't find a unitary.)
*/
State* ApplyGate (State* in, Gate* g) {
  map<index_t, complex<float_t> >::iterator it;
  State* answer = new State;
  for (it = in->data.begin(); it != in->data.end(); it++) {
    State* tmp = ApplyGateToBasis (it->first, g);
    if (tmp == NULL) return NULL;
    complex<float_t> foo = (*it).second;
    *tmp *= foo;
    *answer += *tmp;
    delete tmp;
  }
  return answer;
}

complex<float> *getGateMatrix(string gateName){
  complex<float> * ret;
  if (gateName.compare("H")     == 0){
    ret = new complex<float>[4];
    ret[0]=  1 ; ret[2]=  1;
    ret[1]=  1 ; ret[3]= -1;
  }
  else if (gateName.compare("T")     == 0){
    ret = new complex<float>[4];
    ret[0]=  0 ; ret[2]=  1;
    ret[1]=  1 ; ret[3]=  0;
  }
  else if (gateName.compare("F")     == 0){
    ret = new complex<float>[16];
    ret[0 ]=  1 ; ret[4 ]=  0; ret[8 ]=  0 ; ret[12]=  0;
    ret[1 ]=  0 ; ret[5 ]=  0; ret[9 ]=  1 ; ret[13]=  0;
    ret[2 ]=  0 ; ret[6 ]=  1; ret[10]=  0 ; ret[14]=  0;
    ret[3 ]=  0 ; ret[7 ]=  0; ret[11]=  0 ; ret[15]=  1;
  } else {
    ret = NULL;
  }
  return ret;
}
