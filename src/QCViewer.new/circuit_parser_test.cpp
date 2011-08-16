#include "circuit_parser_utils.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
circuit_list_t *parse_circuits(string input);
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
	circuit_list_t * node = parse_circuits(input);
	if (node == NULL){
		cout << "NULL NODE" << endl;
		return 0;
	}
	node->print();
	return 0;
}

