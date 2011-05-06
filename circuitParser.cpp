#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream> //TODO: FOR TESTING
#include "circuitParser.h"
#include "circuit.h"

using namespace std;

//TODO: Why is there an error with mod5d4.tfc if I do not clean up the strings?

void tokenize(string str, vector<string>& tokens, const char* delimiters = " ")
{
  tokens.clear();
  char* tok;
  tok = strtok(const_cast<char*>(str.c_str()),delimiters);
  while (tok != NULL){
        tokens.push_back(string(tok));
        tok = strtok (NULL,delimiters);
    } 
}

string popFristToken(string str, string& retTok, const char* delimiters = " ")
{
  char *tok;
  string result;
  tok = strtok(const_cast<char*>(str.c_str()),delimiters);
  result = str.substr(strlen(tok));
  while(!result.empty()&&isspace(result.at(0))){
    result.erase(result.begin());
  }
  retTok = string(tok);
  return result;
}

void cleanVars(string& vars){
  for(int i = 0; i < vars.size(); i++){
    if ( vars.at(i) != ','&& vars.at(i) != '.' && !isalnum(vars.at(i))) vars.erase(vars.begin()+i); 
  } 
}

bool parseNames(string str, vector<string>& names){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens,",");
  for(int i = 0; i < tokens.size(); i++){
    names.push_back(tokens.at(i));  
  }
  return true;
}

bool parseLineNumbers(string str, vector<int>& nums, Circuit& circ){
  cleanVars(str);
  vector <string> tokens;
  tokenize(str,tokens, ",");
  for(int i = 0; i < tokens.size(); i++){
    for(int j = 0; j < circ.lNames.size(); j++){
      if (tokens.at(i).compare(circ.lNames.at(j))==0){
        nums.push_back(j);
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
  for(int i = 0; i < tokens.size(); i++){
    circ.constants.push_back(atoi(tokens.at(i).c_str()));  
  }
  return true;
}

void addGate (Circuit &circ, string first, string line){
  Gate newGate;
  newGate.name = first;
  parseLineNumbers(line,newGate.inputs,circ);
  circ.gates.push_back(newGate);
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
      lNamesDone = parseNames(line,circ.lNames); 
      continue;
    }
    if (ident.compare(".ol")==0){
      parseNames(line,circ.outputLabels); 
      continue;
    }
    if (ident.compare(".i")==0){
      parseLineNumbers(line,circ.inputs,circ);
      continue;
    }
    if (ident.compare(".o")==0){
      parseLineNumbers(line,circ.outputs,circ);
      continue;
    }
    if (ident.compare(".c")==0){
      parseConstants(line,circ);
      continue;
    }
    if (ident.compare("BEGIN")==0){
      parseGates(in,circ);
    }
  }
  in.close();
  return circ;
}
