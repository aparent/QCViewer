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

Authors: Alex Parent, Jacob Parker
---------------------------------------------------------------------*/

#ifndef STATE__INCLUDED
#define STATE__INCLUDED
#include <map>
#include <complex>
#include <stdint.h>
#include "types.h"

// If it is desired to up the accuracy later, or change the maximum
// qubit size, change this.
typedef std::map<index_t, std::complex<float_type> >  StateMap;

class State
{
public:
    State ();
    State (index_t);
    State (std::complex<float_type> amp, index_t bits);

    void print();

    std::complex<float_type> getAmplitude (index_t bits);
    const State& operator+= (const State &r);
    const State& operator-= (const State &r);
    const State& operator*= (const std::complex<float_type>);
    void normalize();
    //! Simulates a measurment of a specified bit in the computational basis
    void measure(int bit);
    unsigned int numBits() const;

    index_t dim;
    StateMap data;
};

State kron (const State &l,const State &r);
#endif // STATE__INCLUDED
