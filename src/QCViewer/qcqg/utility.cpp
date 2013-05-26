/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011  Alex Parent and The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

See also 'ADDITIONAL TERMS' at the end of the included LICENSE file.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

QCViewer is a trademark of the of the The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

Authors: Alex Parent, Jacob Parker, Marc Burns
---------------------------------------------------------------------*/

#include "utility.h"
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <iostream>

using namespace std;

void printIntBin(index_t M)
{
    printf(BYTETOBINARYPATTERN " " BYTETOBINARYPATTERN " " BYTETOBINARYPATTERN " " BYTETOBINARYPATTERN " " BYTETOBINARYPATTERN " " BYTETOBINARYPATTERN " " BYTETOBINARYPATTERN " " BYTETOBINARYPATTERN,
           BYTETOBINARY(M>>56),BYTETOBINARY(M>>48),BYTETOBINARY(M>>40), BYTETOBINARY(M>>32),
           BYTETOBINARY(M>>24),BYTETOBINARY(M>>16),BYTETOBINARY(M>>8), BYTETOBINARY(M));
}


int ipow(int base, int exp)
{
    int result = 1;
    while (exp) {
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
    }
    return result;
}

string intToString(int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

string floatToString(float_type i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

string sToUpper(string data)
{
    std::transform(data.begin(), data.end(), data.begin(), ::toupper);
    return data;
}

//Sets the bit at position reg to 1
unsigned int SetRegister (unsigned int bits, unsigned int reg)
{
    return bits | 1<<reg;
}

unsigned int invertBits (unsigned int bits, unsigned int numBits)
{
    unsigned int ret = 0;
    for (unsigned int i = numBits; i>0; i --) {
        if (GetRegister(bits,i-1)) ret = SetRegister(ret,numBits-i);
    }
    return ret;
}

//Sets the bit at position reg to 0
unsigned int UnsetRegister (unsigned int bits, unsigned int reg)
{
    return bits & ~(1<<reg);
}

//Returns the value of the bit at position reg
index_t GetRegister (index_t bits, index_t reg)
{
    return (bits & (1 << reg)) >> reg;
}

unsigned int floorLog2(unsigned int n)
{
    if (n == 0)
        return -1;
    int pos = 0;
    if (n >= 1<<16) {
        n >>= 16;
        pos += 16;
    }
    if (n >= 1<< 8) {
        n >>=  8;
        pos +=  8;
    }
    if (n >= 1<< 4) {
        n >>=  4;
        pos +=  4;
    }
    if (n >= 1<< 2) {
        n >>=  2;
        pos +=  2;
    }
    if (n >= 1<< 1) {
        pos +=  1;
    }
    return pos;
}

unsigned int bitcount(unsigned int n)
{
    unsigned int tot = 0;
    for (unsigned int i = 1; i <= n; i = i<<1) {
        if (n & i)
            ++tot;
    }
    return tot;
}

index_t ExtractBits (index_t target, index_t bitstring)
{
    index_t output = 0;
    unsigned int pos = 0;
    for (unsigned int i = 0; i < 64; i++) { //XXX ug 64bit specific replace with sizeof magics?
        if (GetRegister(bitstring,i)) {
            if (GetRegister(target,i)) {
                output = SetRegister (output, pos);
            }
            pos++;
        }
    }
    return output;
}

