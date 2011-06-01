#include "simulation.h"
#include "state.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
  if (argc != 2) return 0;
  string in = argv[1];
  stateVec ket = getStateVec(in,true);
	state * s = new state(&ket);
	s = applyGate(s,Gate*)
}

