
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
#include "circuit.h"
#include "simulate.h"
#include <cmath>
#include <iostream>
#include <string>
#include "utility.h"
#include "gates/UGateLookup.h"
#include "circuit.h"

using namespace std;

gateMatrix getGateMatrix(Gate*);//defined below

/*
	This function takes an input quantum state and a gate.
	It splits the input superposition into basis states (in the computational basis,
	i.e. into class bit strings.) It runs the gate on each of these, and collates
	the answers (weighed appropriately) into the answer.
	NULL on error (couldn't find a unitary.).
	The application of the gate is left up to the gate class.  This allows per gate class
	optimization if nessicary. (For a good example of how this works see gates/UGate.cpp
*/
#include "subcircuit.h"
State ApplyGate (const State &in, const Gate *g)
{
    if (g->type==Gate::SUBCIRC) {
        return ((Subcircuit*)g)->applySubcirc(in);
    }
    State answer;
    answer.dim = in.dim;
    for (StateMap::const_iterator it = in.data.begin(); it != in.data.end(); ++it) {
        State tmp = g->applyToBasis(it->first);
        complex<float_type> foo = (*it).second;
        tmp *= foo;
        answer += tmp;
    }
    return answer;
}

State ApplyCircuit (const State &in, const Circuit &circ)
{
    State s = in;
    for (unsigned int i = 0; i < circ.numGates(); i++) {
        s = ApplyGate(s,circ.getGate(i));
    }
    return s;
}
