#include "GateParserUtils.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
gate_node *parse_gates(string input);
void print_gate_node(gate_node *n);
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
	gate_node * node = parse_gates(input);
	if (node == NULL){
		cout << "NULL NODE" << endl;
		return 0;
	}
	cout << "---------------------------------------" << endl;
	print_gate_node(node);
	cout << "---------------------------------------" << endl;
	return 0;
}

void print_matrix_row(row_terms *n){
	cout << *n->val;
	if (n->next != NULL){
		cout << " , "; 
		print_matrix_row(n->next);
	}
}

void print_gate_matrix(matrix_row *n){
	print_matrix_row(n->terms);
	cout << endl; 
	if (n->next != NULL) print_gate_matrix(n->next);
}

void print_gate_node(gate_node *n){
	cout << "NAME: " << n->name << endl;
	cout << "SYMBOL: " << n->symbol << endl;
	cout << "Matrix: " << endl;
	print_gate_matrix(n->row);
	if (n->next != NULL) print_gate_node(n->next);
}

