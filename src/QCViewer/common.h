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

#ifndef __COMMON__INCLUDED
#define __COMMON__INCLUDED

#ifndef NULL
#define NULL 0
#endif

#include <stdint.h>
#include <vector>

class Selection
{
public:
    Selection();
    ~Selection();
    Selection(uint32_t n);
    Selection(uint32_t n, std::vector<Selection>* s);
    uint32_t gate;
    std::vector<Selection> * sub;
};

class gateRect
{
public:
    gateRect();
    ~gateRect();
    double x0, y0;
    double width, height;
    std::vector<gateRect> * subRects;
};

class LayoutColumn
{
public:
    LayoutColumn (uint32_t g, double p) : lastGateID(g), pad(p) {}
    uint32_t lastGateID;
    double pad;
    gateRect bounds;
    std::vector<LayoutColumn> *subCols;
};


#endif
