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


#ifndef DRAW__INCLUDED
#define DRAW__INCLUDED

#include <cairo.h>
#include <string>
#include <vector>
#include <stdint.h>

#include "QCLib/circuit.h"
#include "QCLib/common.h"
#include "QCLib/draw_common.h"



void init_fonts();
cairo_rectangle_t get_circuit_size (Circuit *c, std::vector<LayoutColumn>&, double* wirestart, double* wireend, double scale);
int pickRect (const std::vector<gateRect> &rects, double x, double y, std::vector<int> &selections);
std::vector<Selection> pickRects (const std::vector<gateRect> &rects, const gateRect &s);
int pickWire (double);

#endif // DRAW__INCLUDED
