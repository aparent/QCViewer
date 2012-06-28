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
/*! \file circuitDrawer.h
    \brief Defines the circuit drawing class for rendering images of circuits
*/

#ifndef CIRCUITDRAWER_H
#define CIRCUITDRAWER_H

#include <cairo.h>
#include <vector>
#include <string>
#include "common.h"
#include "circuit.h"
#include "draw_constants.h"

class CircuitDrawer
{
public:
    std::vector<gateRect> drawCirc (cairo_t*, Circuit&, double& , double& , bool ) const;
    std::vector<gateRect> draw (cairo_t*, Circuit&, bool, bool, cairo_rectangle_t, double, double, double, const std::vector<Selection>&, cairo_font_face_t *) const;
    cairo_rectangle_t getCircuitSize (Circuit& c, double&, double&, double, cairo_font_face_t*) const;
    void savepng (Circuit&, std::string, cairo_font_face_t*);
    void savesvg (Circuit&, std::string, cairo_font_face_t*);
    void saveps  (Circuit&, std::string, cairo_font_face_t*);
private:
    void drawbase (cairo_t*, Circuit&, double, double, double, double) const;
    void drawArchitectureWarnings (cairo_t*, const std::vector<gateRect>&, const std::vector<int>&) const;
    void drawParallelSectionMarkings (cairo_t*, const std::vector<gateRect>&, int, const std::vector<int>&) const;
    void drawPWire (cairo_t*, double, int) const;
    void drawSelections (cairo_t* , const std::vector<gateRect>& , const std::vector<Selection>&) const;

    void write_to_png (cairo_surface_t*, std::string) const;
    cairo_surface_t* make_ps_surface (std::string, cairo_rectangle_t) const;
};


#endif
