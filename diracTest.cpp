#include "dirac.h"
#include "parseNode.h"
#include <iostream>
using namespace arma;

int main(int argc, char* argv[]){
	string in = argv[1];
  parseNode * parseTree = parseDirac(in);
	cout << "Finished Parsing" << endl;
	cout << printTree(parseTree) << endl;
  diracTerm result =  evalTree(parseTree);
	arma::cx_mat ket = result.vecValue;
	cout << ket << endl;
	for(int i=0;i<ket.n_rows;i++){
		cout << i << " " << real(ket(i,0))*real(ket(i,0)) << endl;
	}
	cout << endl;
}
