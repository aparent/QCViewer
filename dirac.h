#include <armadillo>
#include <iostream>//for error messages
#include <string>
#include <string.h>
#include <cmath>
#include "diracParser.h"
#include "parseNode.h"

using namespace std;
using namespace arma;

struct diracTerm{
	int type;
	cx_mat vecValue; 
	cx_double numValue; 
};

parseNode *parseDirac(string input); //Defined in diracParser.y
string printTree(parseNode *node);
diracTerm evalTree(parseNode *node);
