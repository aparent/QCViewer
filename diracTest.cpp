#include "dirac.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
	if (argc != 2) return 0;
	string in = argv[1];
	stateVec *ket = getStateVec(in,true);
	if (ket == NULL) return 0;
	for(unsigned int i=0;i<ket->dim;i++){
		cout << ket->data[i]  << endl;
	}
	return 0;
}
