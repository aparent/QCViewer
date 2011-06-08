#include "circuitParser.h"
#include "TFCLexer.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
int main(){
	string file = "testCircuits/simtest2.tfc";
	vector<TFCToken> *tokens = lexCircuit(file);
  vector<TFCToken>::iterator it;
  for (it = tokens->begin(); it != tokens->end(); it++) {
		cout << "Type: "<< "|" << it->type <<"|" << " Value: " <<"|" << it->value << "|" << endl;
	}
  Circuit *c = parseCircuit(file);
	cout << "Lines: " << c->numLines() << endl;
  return 0;
}
