#include "dirac.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
	if (argc != 2) return 0;
	string in = argv[1];
	stateVec ket = getStateVec(in,false);
	for(unsigned int i=0;i<ket.dim;i++){
		cout << ket.data[i]  << endl;
	}
}
