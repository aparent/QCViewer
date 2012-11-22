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

#include "state.h"
#include "utility.h" // ipow floorLog2
#include <iostream>
#include <ctime> //for seed values
#include <cstdlib>
#include <limits>
using namespace std;

State::State ()
{
    dim = 0;
}

State::State (index_t n_dim)
{
    dim = n_dim;
}

State::State (complex<float_type> amp, index_t bits)
{
    if(amp!=complex<float_type>(0)) {
        data[bits] = amp;
    }
}

complex<float_type> State::getAmplitude (index_t bits)
{
    if (data.find(bits) == data.end())
        return 0;
    else
        return data[bits];
}

const State& State::operator+= (const State& r)
{
    map<index_t, complex<float_type> >::const_iterator it;
    for (it = r.data.begin(); it != r.data.end(); ++it) {
        if (it->second != complex<float_type>(0)) {
            data[it->first] += it->second;
            float_type e = 10*numeric_limits<float_type>::epsilon();
            if (abs(data[it->first]) < e ) {
                data.erase(it->first);
            }
        }
    }
    return *this;
}

const State& State::operator-= (const State& r)
{
    map<index_t, complex<float_type> >::const_iterator it;
    for (it = r.data.begin(); it != r.data.end(); ++it) {
        if (it->second != complex<float_type>(0)) {
            data[it->first] -= it->second;
            float_type e = 10*numeric_limits<float_type>::epsilon();
            if (abs(data[it->first].real()) < e) {
                data.erase(it->first);
            }
        }
    }
    return *this;
}

const State& State::operator*= (const complex<float_type> x)
{
    map<index_t, complex<float_type> >::const_iterator it;
    for (it = data.begin(); it != data.end(); ++it) {
        data[it->first] *= x;
    }
    return *this;
}

unsigned int State::numBits() const
{
    return floorLog2(dim);
}

State kron (const State& l, const State& r)
{
    StateMap::const_iterator it_r;
    StateMap::const_iterator it_l;
    State ret;
    for (it_r = r.data.begin(); it_r != r.data.end(); ++it_r) {
        for (it_l = l.data.begin(); it_l != l.data.end(); ++it_l) {
            ret.data[(it_r->first << l.numBits())|(it_l->first)] = it_r->second*it_l->second;
        }
    }
    ret.dim = l.dim*r.dim;
    return ret;
}

void State::normalize()
{
    StateMap::const_iterator it;
    float_type norm = 0;
    float_type real;
    float_type imag;
    for (it = data.begin(); it != data.end(); ++it) {
        real = it->second.real ();
        imag = it->second.imag ();
        norm += real*real + imag*imag;
    }
    norm = sqrt(norm);
    for (it = data.begin(); it != data.end(); ++it) {
        data[it->first] = (it->second) / complex<float_type>(norm,0);
    }
}

void State::measure(int bit)
{
    double prob1 = 0;
    StateMap::const_iterator it;
    for (it = data.begin(); it != data.end(); ++it) {
        if (GetRegister (it->first, bit))
            prob1 += norm(it->second);
    }
    double ran = ((double)rand()/(double)RAND_MAX);
    if (ran > prob1) {
        for (it = data.begin(); it != data.end(); ++it) {
            if (GetRegister(it->first, bit))
                data.erase(it->first);
        }
    } else {
        for (it = data.begin(); it != data.end(); ++it) {
            if (!GetRegister(it->first, bit))
                data.erase(it->first);
        }
    }
    normalize();
}

void State::print()
{
    map<index_t, complex<float_type> >::const_iterator it;
    for (it = data.begin(); it != data.end(); ++it) {
        printIntBin(it->first);
        cout << ":" << it->second << endl;
    }
}
