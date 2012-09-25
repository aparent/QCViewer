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

Authors: Alex Parent
---------------------------------------------------------------------*/


#ifndef DRAW_COMMON_H
#define DRAW_COMMON_H

#include <cairo.h>
#include <pango/pangocairo.h>
#include <vector>
#include "types.h"
#include "gate.h"
#include "common.h"

class Colour
{
public:
    Colour () : r(0.0),g(0.0),b(0.0),a(0.0) {}
    Colour (double rr, double gg, double bb, double aa) : r(rr), g(gg), b(bb), a(aa) {}
    double r, g, b, a;
};

PangoLayout* create_text_layout(cairo_t *cr,std::string label, double &w, double &h);
gateRect combine_gateRect (const gateRect &a, const gateRect &b);
double wireToY (uint32_t x);
void drawWire (cairo_t *cr, double x1, double y1, double x2, double y2);
void drawRect (cairo_t *cr, gateRect r, Colour outline, Colour fill);
#endif


