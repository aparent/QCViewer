#include <fstream>
#include <string.h>
#include <vector>
#include <iostream> //TODO: FOR TESTING
#include "circuitParser.h"
#include "circuit.h"

using namespace std;


void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ")
{
  tokens.clear();
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

bool parseLineNames(vector <string> tokens, Circuit& circ){
  for(int i = 0; i < tokens.size(); i++){
    char* cstr = new char [tokens.at(i).size()+1];
    strcpy(cstr, tokens.at(i).c_str());
    circ.lNames.push_back(string(cstr));  
  }
  return true;
}

Circuit parseCircuit (string file){ 
  ifstream in(file.data());
  Circuit circ;
  vector <string> tokens;
  string line;
  bool lNamesDone = false;
  while(!in.eof()){
    getline (in,line);
    if (line.empty()) continue;
    if ( line.at(0) != '.' && !isalnum(line.at(0))) continue; //Ignore symbols other then "."
    tokenize(line,tokens, " ");
    if (tokens.at(0).compare(".v")==0){
      tokenize(string(tokens.at(1).data()),tokens, ",");
      lNamesDone = parseLineNames(tokens,circ); 
    }
  }
  in.close();
  return circ;
}
