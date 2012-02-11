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

#include "QCLib/gate.h"
#include "QCLib/utility.h"
#include <cmath>
#include <complex>

#ifndef M_PI
#define M_PI 3.14159265
#endif

RGate::RGate(float_type n_rot, Axis a) : rot(n_rot)
{
    drawType = DEFAULT;
    type = RGATE;
    axis = a;
    loop_count = 1;
}

Gate* RGate::clone() const
{
    RGate* g = new RGate(rot,axis);
    g->controls = controls;
    g->targets = targets;
    return g;
}

string RGate::getName() const
{
    switch (axis) {
    case RGate::X:
        return "RX(" + floatToString(rot) + ")";
    case RGate::Y:
        return "RY(" + floatToString(rot)+")";
    case RGate::Z:
        return "RZ(" + floatToString(rot)+")";
    default:
        return "R";
    }
}

/* RGate simulation implimentation */
State RGate::applyToBasis(index_t bitString) const
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

State RGate::ApplyU (index_t bits) const
{
    State answer;
    switch (axis) {
    case X: {
        float_type cosr = cos (M_PI*rot);
        float_type sinr = sin (M_PI*rot);
        if (GetRegister (bits, targets.at (0))) {
            answer = State (complex<float_type>(sinr,0),BuildBitString(bits,0));
            answer += State (complex<float_type>(cosr,0),BuildBitString(bits,1));
        } else {
            answer = State (complex<float_type>(-cosr,0),BuildBitString(bits,0));
            answer += State (complex<float_type>(sinr,0),BuildBitString(bits,1));
        }
    }
    break;
    case Y: {
        float_type cosr = cos (M_PI*rot);
        float_type sinr = sin (M_PI*rot);
        if (GetRegister (bits, targets.at (0))) {
            answer = State (complex<float_type>(0,-sinr),BuildBitString(bits,0));
            answer += State (complex<float_type>(cosr,0),BuildBitString(bits,1));
        } else {
            answer = State (complex<float_type>(-cosr,0),BuildBitString(bits,0));
            answer += State (complex<float_type>(0,sinr),BuildBitString(bits,1));
        }
    }
    break;
    case Z: {
        if (GetRegister (bits, targets.at(0))) {
            answer = State(exp(complex<float_type>(0,M_PI*rot)), bits);// XXX: verify do not need buildbitstring
        } else {
            answer = State(1, bits);
        }
    }
    break;
    }
    return answer;
}


index_t RGate::BuildBitString (index_t orig, unsigned int ans) const
{
    unsigned int output = orig;
    if (ans) {
        output = SetRegister (output, targets.at(0));
    }  else {
        output = UnsetRegister (output, targets.at(0));
    }
    return output;
}

void RGate::set_axis (Axis a)
{
    axis = a;
}

RGate::Axis RGate::get_axis () const
{
    return axis;
}

void RGate::set_rotVal (float_type r)   // XXX: remove float_type, consildate this stuff!!
{
    rot = r;
}

float_type RGate::get_rotVal () const
{
    return rot;
}

