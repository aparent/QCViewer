#ifndef UGATE_LOOKUP_INC
#define UGATE_LOOKUP_INC

#include <complex>
#include <string>
#include "../gate.h" //for gateMatrix 

//Function for looking up the matrix of a given ugate
//Will return null if no matrix is found
gateMatrix *UGateLookup(string name);
//adds a matrix and name
void UGateLoad(string name, gateMatrix mat);

void UGateSetup(); //TODO: Should be replaced with a gate library loaded at runtime
#endif
