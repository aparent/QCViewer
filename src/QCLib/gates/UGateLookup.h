#ifndef UGATE_LOOKUP_INC
#define UGATE_LOOKUP_INC

#include <string>
#include <vector>
#include "../gate.h" //for gateMatrix
#include "GateParserUtils.h"

gate_node *parse_gates(std::string input);
//Function for looking up the matrix of a given ugate Will return null if no matrix is found
gateMatrix *UGateLookup(std::string name);
std::vector<std::string> UGateNames();

//adds a matrix and name
void UGateLoad(std::string name, gateMatrix mat);

void UGateSetup();
#endif
