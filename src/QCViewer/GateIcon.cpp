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


#include <iostream>

#include "GateIcon.h"
#include "draw.h"
#include "draw_internal.h"

#define G_BUTTON_SIZE 25

using namespace std;

GateIcon::GateIcon () : type(DEFAULT)
{
    set_size_request (G_BUTTON_SIZE,G_BUTTON_SIZE);
}

GateIcon::GateIcon (GateType t) : type(t)
{
    set_size_request (G_BUTTON_SIZE,G_BUTTON_SIZE);
}

GateIcon::GateIcon (string s,string d) : type(DEFAULT), symbol(s), dname(d)
{
    set_size_request (G_BUTTON_SIZE,G_BUTTON_SIZE);
}

bool GateIcon::on_expose_event(GdkEventExpose* event)
{
    (void)event; // placate compiler..
    Glib::RefPtr<Gdk::Window> window = get_window();
    if(window) {
        double scale = 0.90; // drawing to the borders tends to look bad in buttons
        Gtk::Allocation allocation = get_allocation();
        const double width = allocation.get_width();
        const double height = allocation.get_height();
        Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
        const double xc = width/2.0/scale;
        const double yc = height/2.0/scale;
        cr->scale (scale, scale);
        switch (type) {
        case NOT:
            drawNOT (cr->cobj(), xc, yc, min(width/2.0, height/2.0));
            break;
        case R:
            drawShowRotation (cr->cobj(), xc, yc, min(width/2.0, height/2.0));
            break;
        case SWAP:
            drawShowFred (cr->cobj(), width/scale, height/scale);
            break;
        case DEFAULT:
            drawShowU (cr->cobj (), xc, yc, min(width, height), dname);
        }
    }
    return true;
}
