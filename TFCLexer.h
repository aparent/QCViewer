#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum TFCType {SEC_START, SEC_END, VAR_NAME, GATE_INPUT, GATE_INPUT_N};

struct TFCToken{
	TFCType		type;
	string 		value;
	TFCToken(TFCType type, string value):type(type), value(value) { }
};

vector<TFCToken> *lexCircuit(string fileName);
