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

/*! \file utility.h
    \brief Utility functions

*/

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

/*!
 \brief Returns the floored form of the binary logarithm for a 32 bit integer.

 -1 is returned if ''n'' is 0.
*/
unsigned int floorLog2(unsigned int n);

int ipow(int base, int exp);
std::string intToString(int i);
std::string floatToString(float_type i);
std::string sToUpper(std::string);

/* Bit set/get functions */
/*!
	\brief Sets the bit at position reg to 1
*/
unsigned int SetRegister (unsigned int bits, unsigned int reg);
/*!
  \brief Sets the bit at position reg to 0
*/
unsigned int UnsetRegister (unsigned int bits, unsigned int reg);
/*!
  \brief Returns the value of the bit at position reg
*/
index_t GetRegister (index_t bits, index_t reg);

unsigned int invertBits (unsigned int bits, unsigned int numBits);

unsigned int  bitcount(unsigned int);
index_t ExtractBits (index_t target, index_t bitString);

#endif
