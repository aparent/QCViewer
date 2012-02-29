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
#include "QCLib/draw_constants.h"

using namespace std;

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

void RGate::draw(cairo_t *cr,double &xc,double &maxX, vector <gateRect> &rects) const
{
    uint32_t minw, maxw;
    string name = getName();
    vector<Control> dummy;
    minmaxWire (dummy, targets, minw, maxw); // only the targets
    // (XXX) need to do a  check in here re: target wires intermixed with not targets.

    double dw = wireToY(1)-wireToY(0);
    double yc = (wireToY (minw)+wireToY(maxw))/2;//-dw/2.0;
    double height = dw*(maxw-minw+Upad);

    // get width of this box
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, name.c_str(), &extents);
    double width = extents.width+2*textPad;
    if (width < dw*Upad) {
        width = dw*Upad;
    }
    gateRect rect = drawControls (cr, xc-radius+width/2.0);
    cairo_rectangle (cr, xc-radius, yc-height/2, width, height);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);
    cairo_rectangle (cr, xc-radius, yc-height/2, width, height);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, thickness);
    cairo_stroke(cr);

    double x = (xc - radius + width/2) - extents.width/2 - extents.x_bearing;
    double y = yc - extents.height/2 - extents.y_bearing;
    cairo_move_to(cr, x, y);
    cairo_show_text (cr, name.c_str());
    gateRect r;
    r.x0 = xc - thickness-radius;
    r.y0 = yc -height/2 - thickness;
    r.width = width + 2*thickness;
    r.height = height + 2*thickness;
    rects.push_back(combine_gateRect(rect, r));
    maxX = max (maxX, r.width);
}
