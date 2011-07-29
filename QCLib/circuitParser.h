#ifndef CIRCUIT_PARSER
#define CIRCUIT_PARSER
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include "circuit.h"
#include "utility.h"
#include "TFCLexer.h"

using namespace std;

Circuit *parseCircuit (string file);
void saveCircuit(Circuit *circ, string filename);

#endif
