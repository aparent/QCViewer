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

Authors: Alex Parent, Jakub Parker
---------------------------------------------------------------------*/


//Implimentation code for the UGate
#include "../gate.h"
#include "UGateLookup.h"
#include "utility.h"
#include <complex>
#include <iostream>
using namespace std;

UGate::UGate(string n_name) : name(n_name)
{
    drawType = DEFAULT;
    type = UGATE;
}

Gate* UGate::clone()
{
    UGate *g = new UGate(name);
    g->controls = controls;
    g->targets = targets;
    g->drawType = drawType;
    return g;
}

string UGate::getName() const
{
    if (drawType == NOT) {
        return "T"+intToString(controls.size());
    } else {
        return name;
    }
}
void UGate::setName(string n_name)
{
    name = n_name;
}

/*
  Applies the gate to a single basis state sim does this classically
	for all basis states in the superposition.  Note this can return a
	superpostion.
*/
State UGate::applyToBasis(index_t bitString) const
{
    // First, make sure all of the controls are satisfied.
    bool ctrl = true;
    for (unsigned int i = 0; i < controls.size(); i++) {
        Control c = controls[i];
        int check = GetRegister (bitString, c.wire);
        if (!c.polarity != check) {
            ctrl = false; // control line not satisfied.
            break;
        }
    }
    if (ctrl) {
        return ApplyU (bitString);
    } else {
        return State (1, bitString);// with amplitude 1 the input bitString is unchanged
    }
}

// Takes care of actually applying the matrix by indexing to the correct matrix coloum
State UGate::ApplyU (index_t bits) const
{
    unsigned int input = ExtractInput (bits);
    // now, go through all rows of the output from the correct column of U
    State answer;
    gateMatrix *matrix = UGateLookup(name);
        if (matrix == NULL){ 
					cerr << "Matrix not found!" << endl;
					return State();
    }
    for (unsigned int i = 0; i < matrix->dim; i++) {
        if (matrix->data[input*matrix->dim+i] != complex<float_type>(0)) {
            answer += State(matrix->data[input*matrix->dim+i], BuildBitString (bits, i));
        }
    }
    return answer;
}

/*
	This function takes an input bitstring and the mapping of targets and returns
	the input to the gate, for the purpose of indexing into its matrix.
*/
unsigned int UGate::ExtractInput (index_t bitString) const
{
    unsigned int input = 0;
    for (unsigned int i = 0; i < targets.size(); i++) {
        if (GetRegister (bitString, targets.at(i))) {
            input = SetRegister (input, i);
        }
    }
    return input;
}

/*
	This function takes an input string, the mapping of targets, and an output for those targets
	and produces the output bitstring
*/
index_t UGate::BuildBitString (index_t orig, unsigned int ans) const
{
    unsigned int output = orig;
    for (unsigned int i = 0; i < targets.size (); i++) {
        if (GetRegister (ans, i)) {
            output = SetRegister (output, targets.at(i));
        } else {
            output = UnsetRegister (output, targets.at(i));
        }
    }
    return output;
}
