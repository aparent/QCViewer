/*--------------------------------------------------------------------
QCViewer
Copyright (C) 2011  Alex Parent and The University of Waterloo,
Institute for Quantum Computing, Quantum Circuits Group

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

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
#include "QCLib/text.h"

class GateIcon : public Gtk::DrawingArea
{
public:
    enum GateType { NOT, R, SWAP, MEASURE, DEFAULT };
    GateIcon ();
    GateIcon (std::string,std::string);
    GateIcon (GateType);
    GateType type;
    std::string symbol;
    std::string dname;
    virtual bool on_expose_event(GdkEventExpose* event);
private:
    TextEngine textEngine;
    void drawWire (cairo_t *cr, double x1, double y1, double x2, double y2);
    void drawShowU (cairo_t *cr, double xc, double yc, double width, std::string name);
    void drawShowMeasure (cairo_t *cr, double xc, double yc, double width);
    void drawShowNOT (cairo_t *cr, double xc, double yc, double radius);
    void drawShowRotation (cairo_t *cr, double xc, double yc, double radius);
    void drawX (cairo_t *cr, double xc, double yc, double radius);
    void drawShowFred (cairo_t *cr, double width, double height);
};

#endif
