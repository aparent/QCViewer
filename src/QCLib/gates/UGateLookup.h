#ifndef UGATE_LOOKUP_INC
#define UGATE_LOOKUP_INC

#include <string>
#include "../gate.h" //for gateMatrix
#include "GateParserUtils.h"

gate_node *parse_gates(string input);
//Function for looking up the matrix of a given ugate Will return null if no matrix is found
gateMatrix *UGateLookup(string name);

//adds a matrix and name
void UGateLoad(string name, gateMatrix mat);

void UGateSetup();
#endif
