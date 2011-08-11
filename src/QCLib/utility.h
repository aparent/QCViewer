#ifndef UTILITY_FUNCS
#define UTILITY_FUNCS

#include <string>
#include <vector>
#include "types.h"

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
(byte & 0x80 ? 1 : 0), \
(byte & 0x40 ? 1 : 0), \
(byte & 0x20 ? 1 : 0), \
(byte & 0x10 ? 1 : 0), \
(byte & 0x08 ? 1 : 0), \
(byte & 0x04 ? 1 : 0), \
(byte & 0x02 ? 1 : 0), \
(byte & 0x01 ? 1 : 0)

void printIntBin(index_t);

using namespace std;

unsigned int floorLog2(unsigned int n);
int ipow(int base, int exp);
void tokenize(const string&,
              vector<string>&,
              const string&);
string intToString(int i);
string sToUpper(string);

/* Bit set/get functions */
//Sets the bit at position reg to 1
unsigned int SetRegister (unsigned int bits, unsigned int reg);
//Sets the bit at position reg to 0
unsigned int UnsetRegister (unsigned int bits, unsigned int reg);
//Returns the value of the bit at position reg
index_t GetRegister (index_t bits, index_t reg);

unsigned int  bitcount(unsigned int);
index_t ExtractBits (index_t target, index_t bitString);

#endif
