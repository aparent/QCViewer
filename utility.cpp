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

string intToString(int i){
  stringstream ss;
  ss << i;
  return ss.str();
}
