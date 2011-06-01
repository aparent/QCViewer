#include "circuit.h"
#include "gate.h"
#include "state.h"

/* This function takes an entire input basis vector, the matrix to apply and
   the targets, which describe which bits map to which inputs of the matrix.
   It returns a (usually very sparse) quantum state.
*/
State* ApplyU (index_t bits, vector<int>* targets, Matrix* matrix) {
  unsigned int input_column = 0;
  for (unsigned int i = 0; i < targets.size(); i++) {
    input_column = (input_column<<1) & (bits & 1<<targets[i]); // XXX: VERIFY
  }
  // now, go through all rows of the output from the correct column of U
  State *answer = new State;
  for (unsigned int i = 0; i < matrix.dim(); i++) {
    // somehow use matrix[input_column+i]
    *answer += State (....);
  }
  return answer;
}

/* This function takes a computational basis element and a gate
   and simulates. It returns a quantum state (usually very sparse.)
*/
State* ApplyGateToBasis (index_t bitString, Gate &g) {
  // First, make sure all of the controls are satisfied.
  bool ctrl = true;
  for (unsigned int i = 0; i < g.controls.size(); i++) {
    Control c = g.controls[i];
    index_t check = (bitString & 1>>c.wire); // 0 iff satisfied XXX: VERIFY
    if ((!c.polarity && !check) || (c.polarity && check)) {
      ctrl = false; // control line not satisfied.
      break;
    }
  }
  if (ctrl) {
    return ApplyU (bitString, g.targets, g.matrix);
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
    answer += it->second * (*tmp);
    delete tmp;
  }
  return answer;
}
