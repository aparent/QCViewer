#ifndef UTILITY_FUNCS
#define UTILITY_FUNCS

#include <string>
#include <sstream>
#include <vector>

using namespace std;

int ipow(int base, int exp);

void tokenize(const string&,
              vector<string>&,
              const string&);

string intToString(int i);

#endif
