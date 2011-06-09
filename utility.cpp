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
/**
* Returns the floor form of binary logarithm for a 32 bit integer.
* −1 is returned if ''n'' is 0.
*/
unsigned int floorLog2(unsigned int n) {
	if (n == 0)
		return -1;
	int pos = 0;
	if (n >= 1<<16) { n >>= 16; pos += 16; }
	if (n >= 1<< 8) { n >>=  8; pos +=  8; }
	if (n >= 1<< 4) { n >>=  4; pos +=  4; }
	if (n >= 1<< 2) { n >>=  2; pos +=  2; }
	if (n >= 1<< 1) {           pos +=  1; }
	return pos;
}
