#include "utility.h"
#include <stdio.h>

using namespace std;

void printIntBin(unsigned int M){
	printf(BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN" "BYTETOBINARYPATTERN,
	BYTETOBINARY(M>>24),BYTETOBINARY(M>>16),BYTETOBINARY(M>>8), BYTETOBINARY(M));
}


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
