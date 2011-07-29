#ifndef QC_SIM_INCL
#define QC_SIM_INCL

#include "gate.h"
#include "circuit.h"
#include "state.h"

State ApplyGate (State* in, Gate* g);
State ApplyCircuit (State* in, Circuit* circ);

#endif
