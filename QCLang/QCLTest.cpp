#include "QCLParserUtils.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void printTree(QCLParseNode * node);
QCLParseNode * parseQCL(string input);

int main(int argc, char* argv[]){
	if (argc != 2) return 0;
	string in = argv[1];
	string line,input;
	ifstream myfile (in.c_str());
	if (myfile.is_open()){
		while ( myfile.good() ){
			getline (myfile,line);
			input += line + "\n";
		}
	}
	QCLParseNode * node = parseQCL(input);
	if (node == NULL){
		cout << "NULL NODE" << endl;
		return 0;
	}
	printTree(node);	
	return 0;
}
