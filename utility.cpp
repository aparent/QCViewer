#include "utility.h"
#include <stdio.h>
#include <algorithm>

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

string sToUpper(string data){
	std::transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}

//Sets the bit at position reg to 1
unsigned int SetRegister (unsigned int bits, unsigned int reg) {
  return bits | 1<<reg;
}

//Sets the bit at position reg to 0
unsigned int UnsetRegister (unsigned int bits, unsigned int reg) {
  return bits & ~(1<<reg);
}

//Returns the value of the bit at position reg
unsigned int GetRegister (unsigned int bits, unsigned int reg) {
  return (bits & (1 << reg)) >> reg;
}
