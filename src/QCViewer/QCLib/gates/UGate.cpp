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


//Implimentation code for the UGate
#include "QCLib/gate.h"
#include "UGateLookup.h"
#include "QCLib/utility.h"
#include <complex>
#include <iostream>
#include "QCLib/draw_constants.h"


using namespace std;

UGate::UGate(string n_name) : Gate(), name(n_name)
{
    drawType = DEFAULT;
    type = UGATE;
    loop_count = 1;
    dname_checked = false;
}

Gate* UGate::clone() const
{
    UGate *g = new UGate(name);
    g->controls = controls;
    g->targets = targets;
    g->drawType = drawType;
    g->breakpoint = breakpoint;
    return g;
}

string UGate::getName() const
{
    if (drawType == NOT) {
        return "tof";
    } else {
        return name;
    }
}

string UGate::getDrawName()
{
    if (!dname_checked) {
        gateMatrix *matrix = UGateLookup(name);
        if (matrix == NULL) {
            cerr << "Game matrix not found for " << name  << "!" << endl;
            dname  = "";
        } else {
            dname = matrix->drawName;
        }
    }
    return dname;
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
    if (matrix == NULL) {
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

void UGate::draw(cairo_t *cr,double &xcurr,double &maxX, vector <gateRect> &rects)
{
    gateRect r;
    switch (drawType) {
    case NOT:
        r = drawCNOT (cr, xcurr);
        break;
    case FRED:
        r = drawFred (cr, xcurr);
        break;
    case Gate::DEFAULT:
    default:
        r = drawCU (cr, xcurr);
        break;
    }
    rects.push_back(r);
    maxX = max (maxX, r.width);
}

gateRect UGate::drawFred (cairo_t *cr, uint32_t xc) const
{
    gateRect rect = drawControls (cr, xc);
    uint32_t minw = targets.at(0);
    uint32_t maxw = targets.at(0);
    for (uint32_t i = 0; i < targets.size(); i++) {
        gateRect recttmp = drawX (cr, xc, wireToY(targets.at(i)), radius);
        rect = combine_gateRect(rect, recttmp);
        minw = min (minw, targets.at(i));
        maxw = max (maxw, targets.at(i));
    }
    if (!controls.empty()) {
        drawWire (cr, xc, wireToY (minw)-radius, xc, wireToY (maxw));
    } else {
        drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw));
    }
    return rect;
}

gateRect UGate::drawCNOT (cairo_t *cr, uint32_t xc) const
{
    gateRect rect = drawControls (cr, xc);
    for (uint32_t i = 0; i < targets.size(); i++) {
        gateRect recttmp = drawNOT (cr, xc, wireToY(targets.at(i)), radius);
        rect = combine_gateRect(rect, recttmp);
    }
    return rect;
}

gateRect UGate::drawNOT (cairo_t *cr, double xc, double yc, double radius, bool opaque) const
{
    cairo_set_line_width (cr, thickness);
    // Draw white background
    if (opaque) {
        cairo_set_source_rgb (cr, 1, 1, 1);
        cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
        cairo_fill (cr);
    }
    // Draw black border
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_stroke (cr);

    // Draw cross
    cairo_move_to (cr, xc-radius, yc);
    cairo_line_to (cr, xc+radius, yc);
    cairo_stroke (cr);
    cairo_move_to (cr, xc, yc-radius);
    cairo_line_to (cr, xc, yc+radius);
    cairo_stroke (cr);

    gateRect r;
    r.x0 = xc-radius-thickness;
    r.y0 = yc-radius-thickness;
    r.width = 2*(radius+thickness);
    r.height = r.width;
    return r;
}

gateRect UGate::drawX (cairo_t *cr, double xc, double yc, double radius) const
{
    // Draw cross
    radius = radius*sqrt(2)/2;
    cairo_move_to (cr, xc-radius, yc-radius);
    cairo_line_to (cr, xc+radius, yc+radius);
    cairo_stroke (cr);
    cairo_move_to (cr, xc+radius, yc-radius);
    cairo_line_to (cr, xc-radius, yc+radius);
    cairo_stroke (cr);

    gateRect r;
    r.x0 = xc-radius-thickness;
    r.y0 = yc-radius-thickness;
    r.width = 2*(radius+thickness);
    r.height = r.width;
    return r;
}

gateRect UGate::drawCU (cairo_t *cr, uint32_t xc)
{
    uint32_t minw, maxw;
    string name = getDrawName();
    vector<Control> dummy;
    minmaxWire (dummy, targets, minw, maxw); // only the targets
    // (XXX) need to do a  check in here re: target wires intermixed with not targets.

    double dw = wireToY(1)-wireToY(0);
    double yc = (wireToY (minw)+wireToY(maxw))/2;//-dw/2.0;

    cairo_set_source_rgb (cr, 0, 0, 0);
    double w,h;
    PangoLayout *layout = create_text_layout(cr, name, w, h);
    double height = dw*(maxw-minw+Upad);
    double width = w+2*textPad;
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

    double x = (xc - radius + width/2) - w/2;// - extents.x_bearing;
    double y = yc - height/2; //- extents.y_bearing;
    cairo_move_to(cr, x, y);

    pango_cairo_show_layout (cr, layout);
    g_object_unref(layout);

    gateRect r;
    r.x0 = xc - thickness-radius;
    r.y0 = yc -height/2 - thickness;
    r.width = width + 2*thickness;
    r.height = height + 2*thickness;
    return combine_gateRect(rect, r);
}
