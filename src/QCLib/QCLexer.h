#ifndef QC_LEXER
#define QC_LEXER

#include <vector>
#include <string>

using namespace std;

enum QCType {SEC_START, SEC_END, VAR_NAME, GATE_INPUT, GATE_INPUT_N, GATE_SET};

struct QCToken{
	QCType		type;
	string 		value;
	QCToken(QCType type, string value):type(type), value(value) { }
};

vector<QCToken> *lexCircuit(string fileName);

#endif
