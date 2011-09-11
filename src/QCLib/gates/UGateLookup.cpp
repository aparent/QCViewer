#include "UGateLookup.h"
#include "../state.h" //for float_type
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

map<string,gateMatrix*> gateLib;

std::vector<std::string> UGateNames()
{
  vector<string> result;
  map<string,gateMatrix*>::iterator it; 
  for ( it=gateLib.begin() ; it != gateLib.end(); it++ )
  {
    if (((*it).first).compare("F") != 0){//TODO: this is very special case
      result.push_back((*it).first);
    }
  }
  return result;
}
gateMatrix *UGateLookup(string name){
	if ( gateLib.find(name) == gateLib.end() ){
		cerr << "GATE: " << name << " does not exist" << endl;
		return NULL;
	}
	return gateLib[name];
}

void UGateLoad(string name, gateMatrix *mat){
	if ( gateLib.find(name) == gateLib.end() ) {
		gateLib[name]=mat;
	} else {
		cerr << "WARNING: The gate "<< name << " already exists overwritting..." << endl;
		gateLib[name]=mat;
	}
}

gateMatrix *get_matrix(matrix_row *n){
	matrix_row* temp = n;
	unsigned int numRow=0;
	while (temp){
		temp = temp->next;
		numRow++;
	}
	temp = n;
	while (temp){
		unsigned int numCol = 0;
		row_terms *terms = temp->terms;
		while(terms){
			terms = terms->next;
			numCol++;
		}
		if (numCol != numRow){
			cerr << "Invalid Gate Matrix" << endl;
			return NULL;
		}
		temp = temp->next;
	}
	gateMatrix *ret = new gateMatrix;
	ret->data = new complex<float_type>[numRow*numRow];
	for(unsigned int i = 0; i < numRow; i++){
		row_terms *terms = n->terms;
		for(unsigned int j = 0; j < numRow; j++){
			ret->data[i*numRow + j] = *terms->val;
			terms = terms->next;
		}
		n = n->next;
	}
	ret->dim = numRow;
	return ret;
}

void add_gates(gate_node *n){
	gateMatrix *g =get_matrix(n->row);
	if (g != NULL) UGateLoad(n->symbol,g);
	if (n->next != NULL) add_gates(n->next);
}

void UGateSetup(){
	string line,input;
	ifstream myfile ("gateLib");
	if (myfile.is_open()){
		while ( myfile.good() ){
			getline (myfile,line);
			input += line + "\n";
		}
	}
	gate_node * node = parse_gates(input);
	add_gates(node);
}
