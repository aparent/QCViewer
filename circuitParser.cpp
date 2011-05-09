#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream> //TODO: FOR TESTING
#include "circuitParser.h"
#include "circuit.h"

using namespace std;

//TODO: Why is there an error with mod5d4.tfc if I do not clean up the strings?

void tokenize(const string& str,vector<string>& tokens,const string& delimiters = " "){
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);// Skip delimiters at beginning. 
  string::size_type pos     = str.find_first_of(delimiters, lastPos); // Find first "non-delimiter".
  while (string::npos != pos || string::npos != lastPos){
    tokens.push_back(str.substr(lastPos, pos - lastPos));// Found a token, add it to the vector. 
    lastPos = str.find_first_not_of(delimiters, pos);// Skip delimiters.  Note the "not_of"
    pos = str.find_first_of(delimiters, lastPos); // Find next "non-delimiter"
  }
}

string popFristToken(string str, string& retTok, const char* delimiters = " "){
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  string::size_type pos     = str.find_first_of(delimiters, lastPos);
  retTok = str.substr(lastPos, pos - lastPos);
  if (string::npos != pos) return str.substr(pos);
  return "";
}

void cleanVars(string& vars){
  for(int i = 0; i < vars.size(); i++){
    if ( vars.at(i) != ','&& vars.at(i) != '.' && !isalnum(vars.at(i))){
      vars.erase(vars.begin()+i); 
    }
  } 
}

bool parseLineNames(string str, Circuit& circ){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens,",");
  for(int i = 0; i < tokens.size(); i++){
    circ.addLine(tokens.at(i));
  }
  return true;
}


bool parseInputs(string str, Circuit& circ){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens, ",");
  for(int i = 0; i < tokens.size(); i++){
    for(int j = 0; j < circ.numLines(); j++){
      if (tokens.at(i).compare(circ.getLine(j)->lineName)==0){
        circ.getLine(i)->constant=false;
        continue; 
      }
    }
  }
  return true;
}

bool parseOutputs(string str, Circuit& circ){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens, ",");
  for(int i = 0; i < tokens.size(); i++){
    for(int j = 0; j < circ.numLines(); j++){
      if (tokens.at(i).compare(circ.getLine(j)->lineName)==0){
        circ.getLine(i)->garbage=false;
        continue; 
      }
    }
  }
  return true;
}

bool parseConstants(string str, Circuit& circ){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens, ",");
  for(int i=0,j=0; i < tokens.size() && j < circ.numLines() ; j++ ){
    if (circ.getLine(j)->constant){
      circ.getLine(j)->initValue = atoi(tokens.at(i).c_str());
      i++;
    }
  }
  return true;
}

bool parseOutputNames(string str, Circuit& circ){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens,",");
  for(int i=0,j=0; i < tokens.size() && j < circ.numLines() ; j++ ){
    if (!circ.getLine(j)->garbage){
      circ.getLine(j)->outLabel = tokens.at(i);
      i++;
    }
  }
  return true;
}

bool parseGateInputs(string str, Gate& gate, Circuit& circ){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens, ",");
  bool target = true;
  for(int i = 0; i < tokens.size(); i++){
    for(int j = 0; j < circ.numLines(); j++){
      if (tokens.at(i).compare(circ.getLine(j)->lineName)==0){
        if (target){
          gate.targets.push_back(j);
        }
        else {
          gate.controls.push_back(Control(j,false)); 
        }
        continue; 
      }
    }
  }
  return true;
}

void addGate (Circuit &circ, string first, string line){
  CNOTGate newGate;
  newGate.name = first;
  parseGateInputs(line,newGate,circ);
  circ.addGate(newGate);
}

void parseGates(ifstream& in, Circuit& circ){
  vector <string> tokens;
  string line,first;
  while(!in.eof()){
    getline (in,line);
    if (line.empty()) continue; //Ignore Empty Lines
    if ( line.at(0) != '.' && !isalnum(line.at(0))) continue; //Ignore symbols other then "."
    line = popFristToken(line,first);
    cleanVars(first);
    if (first.compare("END")==0){
      break;
    }
    addGate(circ, first, line);
  }
}

Circuit parseCircuit (string file){ 
  ifstream in(file.data());
  Circuit circ;
  vector <string> tokens;
  string line,ident;
  bool lNamesDone = false;
  while(!in.eof()){
    getline (in,line);
    if (line.empty()) continue; //Ignore Empty Lines
    if ( line.at(0) != '.' && !isalnum(line.at(0))) continue; //Ignore symbols other then "."
    line = popFristToken(line,ident);
    cleanVars(ident);
    if (ident.compare(".v")==0){
      lNamesDone = parseLineNames(line,circ); 
      continue;
    }
    if (ident.compare(".i")==0){
      parseInputs(line,circ);
      continue;
    }
    if (ident.compare(".o")==0){
      parseOutputs(line,circ);
      continue;
    }
    if (ident.compare(".c")==0){
      parseConstants(line,circ);
      continue;
    }
    if (ident.compare(".ol")==0){
      parseOutputNames(line,circ); 
      continue;
    }
    if (ident.compare("BEGIN")==0){
      parseGates(in,circ);
    }
  }
  in.close();
  return circ;
}
