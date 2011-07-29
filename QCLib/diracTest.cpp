#include "dirac.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
	if (argc != 2) return 0;
	string in = argv[1];
	State *ket = getStateVec(in,true);
	if (ket == NULL){
		cout << "NULL STATE" << endl;
		return 0;
	}
	ket->print();
	return 0;
}
