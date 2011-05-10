#include "utility.h"

using namespace std;

int ipow(int base, int exp){
  int result = 1;
  while (exp){
    if (exp & 1) result *= base;
    exp >>= 1;
    base *= base;
  }
  return result;
}

void tokenize(const string& str,vector<string>& tokens,const string& delimiters = " "){
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);// Skip delimiters at beginning. 
  string::size_type pos     = str.find_first_of(delimiters, lastPos); // Find first "non-delimiter".
  while (string::npos != pos || string::npos != lastPos){
    tokens.push_back(str.substr(lastPos, pos - lastPos));// Found a token, add it to the vector. 
    lastPos = str.find_first_not_of(delimiters, pos);// Skip delimiters.  Note the "not_of"
    pos = str.find_first_of(delimiters, lastPos); // Find next "non-delimiter"
  }
}

string intToString(int i){
  stringstream ss;
  ss << i;
  return ss.str();
}
