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


#ifndef DRAW__INCLUDED
#define DRAW__INCLUDED

#include <cairo.h>
#include <string>
#include <vector>
#include <stdint.h>

#include "circuitwidget.h"
#include "QCLib/circuit.h"
#include "common.h"


class Colour
{
public:
    Colour () : r(0.0),g(0.0),b(0.0),a(0.0) {}
    Colour (double rr, double gg, double bb, double aa) : r(rr), g(gg), b(bb), a(aa) {}
    double r, g, b, a;
};

void init_fonts();
cairo_rectangle_t get_circuit_size (Circuit *c, std::vector<LayoutColumn>&, double* wirestart, double* wireend, double scale);
cairo_surface_t* make_png_surface (cairo_rectangle_t ext);
cairo_surface_t* make_svg_surface (std::string, cairo_rectangle_t);
cairo_surface_t* make_ps_surface (std::string, cairo_rectangle_t);
std::vector<gateRect> draw_circuit (Circuit *c, cairo_t* cr, std::vector<LayoutColumn>&, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, std::vector<Selection>);
void write_to_png (cairo_surface_t* surf, std::string filename);
void pickRect (const std::vector<gateRect> &rects, double x, double y, std::vector<int> &selections);
std::vector<Selection> pickRects (const std::vector<gateRect> &rects, const gateRect &s);
void drawRect (cairo_t *cr, gateRect r, Colour outline, Colour fill);
gateRect combine_gateRect (const gateRect &a, const gateRect &b);
int pickWire (double);
double wireToY (uint32_t);

#endif // DRAW__INCLUDED
