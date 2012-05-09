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

/*! \file common.h
    \brief Defines some classes used by many parts of the code
*/

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

/*! \brief Rectangle that defines the area taken up by a gate

	 Can also contain subrectangles in the case where the gate is
	a subcircuit
*/
class gateRect
{
public:
    gateRect();
    //! Frees all subRects.
    void remove();
    //! The x position of the rectangle
    double x0;
    //! The y position of the rectangle
    double y0;
    //! The width of the rectangle
    double width;
    //! The height of the rectangle
    double height;
    //! Contains subrectangles in the case of an expanded subcircuit
    std::vector<gateRect> * subRects;
};

class LayoutColumn
{
public:
    LayoutColumn (uint32_t g) : lastGateID(g) {}
    uint32_t lastGateID;
};


#endif
