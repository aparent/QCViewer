#ifndef TFC_LEXER
#define TFC_LEXER

#include <vector>
#include <string>

using namespace std;

enum TFCType {SEC_START, SEC_END, VAR_NAME, GATE_INPUT, GATE_INPUT_N};

struct TFCToken{
	TFCType		type;
	string 		value;
	TFCToken(TFCType type, string value):type(type), value(value) { }
};

vector<TFCToken> *lexCircuit(string fileName);

#endif
