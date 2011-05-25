#include "dirac.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
	string in = argv[1];
	stateVec ket = getStateVec(in);
	for(int i=0;i<ket.dim;i++){
		cout << ket.data[i]  << endl;
	}
}
