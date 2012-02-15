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

#ifndef GATEICON__INCLUDED
#define GATEICON__INCLUDED
#include <gtkmm.h>
#include <string>

class GateIcon : public Gtk::DrawingArea
{
public:
    enum GateType { NOT, R, SWAP, DEFAULT };
    GateIcon ();
    GateIcon (std::string);
    GateIcon (GateType);
    GateType type;
    std::string symbol;
    virtual bool on_expose_event(GdkEventExpose* event);
};

#endif
