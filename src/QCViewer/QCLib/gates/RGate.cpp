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

#include "QCLib/gate.h"
#include "QCLib/utility.h"
#include <cmath>
#include <complex>

using namespace std;

RGate::RGate(float_type n_rot, Axis a) : Gate(), rot(n_rot)
{
    frac = false;
    numer = 0;
    denom = 0;
    drawType = DEFAULT;
    type = RGATE;
    axis = a;
    loop_count = 1;
}

RGate::RGate(Axis a, int n_numer, int n_denom) : Gate()
{
    frac = true;
    numer = n_numer;
    denom = n_denom;
    rot = (double)numer/(double)denom;
    drawType = DEFAULT;
    type = RGATE;
    axis = a;
    loop_count = 1;
}
shared_ptr<Gate> RGate::clone() const
{
    RGate* g = new RGate(rot,axis);
    g->controls = controls;
    g->targets = targets;
    g->breakpoint = breakpoint;
    return shared_ptr<Gate>(g);
}

string RGate::getName() const
{
    string rotStr;
    if (frac) {
        rotStr = intToString(numer) + " pi" + "/" + intToString(denom);
    } else {
        rotStr = floatToString(rot);
    }
    switch(axis) {
    case RGate::X:
        return "RX(" + rotStr + ")";
    case RGate::Y:
        return "RY(" + rotStr + ")";
    default:
        return "RZ(" + rotStr + ")";
    }
}

std::string RGate::getLatexName()
{
    string rot_str;
    string pi_str;
    pi_str = "\\pi";
    if(frac) {
        if((numer == 1) && (denom ==1)) {
            rot_str = pi_str;
        } else if (numer == 1) {
            rot_str = "\\frac{" + pi_str + "}{" + intToString(denom) + "}";
        } else if (denom == 1) {
            rot_str = intToString(numer) + pi_str;
        } else {
            rot_str = "\\frac{" + intToString(numer) + pi_str + "}{" + intToString(denom) + "}";
        }
    } else {
        if (rot == 1.0) {
            rot_str = pi_str;
        } else {
            rot_str = floatToString(rot) + pi_str;
        }
    }
    std::string str = "R_";
    switch(axis) {
    case RGate::X:
        str += "x";
        break;
    case RGate::Y:
        str += "y";
        break;
    case RGate::Z:
        str += "z";
        break;
    }
    str += "\\left(" + rot_str + "\\right)";
    return str;
}

std::string RGate::getDrawName()
{
    string rot_str;
    const string startTag = "<span font_desc=\"LMMathItalic10 bold 18\">";
    const string endTag =  "</span>";
    if (frac) {
        if ((numer == 1) && (denom ==1)) {
            rot_str = "π";
        } else if (numer == 1) {
            rot_str = "π/" + intToString(denom);
        } else if (denom == 1) {
            rot_str = intToString(numer) + "π";
        } else {
            rot_str = intToString(numer) + "π/" + intToString(denom);
        }
    } else {
        if (rot == 1.0) {
            rot_str = "π";
        } else {
            rot_str = floatToString(rot) + "π";
        }
    }
    rot_str = startTag + rot_str + endTag;
    switch (axis) {
    case RGate::X:
        return "RX(" + rot_str + ")";
    case RGate::Y:
        return "RY(" + rot_str + ")";
    case RGate::Z:
        return "RZ(" + rot_str + ")";
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
