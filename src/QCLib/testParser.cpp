#include "circuitParser.h"
#include "TFCLexer.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
int main(){
	string file = "testCircuits/largeTest.tfc";
	saveCircuit(parseCircuit(file), "test.qc");
  return 0;
}
