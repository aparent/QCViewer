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
#include <cairo-ft.h>
#include <cmath>
#include "QCLib/draw_constants.h"

#ifndef M_PI
#define M_PI 3.141592
#endif


#define G_BUTTON_SIZE 25

//Font stuff
extern FT_Library library;
extern FT_Face ft_face;
extern cairo_font_face_t * ft_default;


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

void drawWire (cairo_t *cr, double x1, double y1, double x2, double y2)
{
    cairo_set_line_width (cr, thickness);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_move_to (cr, x1, y1);
    cairo_line_to (cr, x2, y2);
    cairo_stroke (cr);
}

void drawShowU (cairo_t *cr, double xc, double yc, double width, string name)
{
    cairo_set_font_face (cr,ft_default);
    cairo_set_font_size(cr, 18);
    double w,h;
    PangoLayout *layout = create_text_layout(cr, name, w, h);
    cairo_rectangle (cr, xc - width/2.0, yc- width/2.0, width, width);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_stroke (cr);
    double scale = width/(max(w,h)+textPad);
    cairo_scale(cr,scale,scale);
    double x = (1.0/scale)*(xc) - (1.0/2.0)*w;
    double y = (1.0/scale)*(yc)- (1.0/2.0)*h ;
    cairo_move_to (cr, x, y);
    pango_cairo_show_layout (cr, layout);
    g_object_unref(layout);
}

void drawShowNOT (cairo_t *cr, double xc, double yc, double radius)
{
    cairo_set_line_width (cr, thickness);
    // Draw black border
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_stroke (cr);

    // Draw cross
    drawWire (cr,xc-radius, yc, xc+radius, yc);
    drawWire (cr,xc, yc-radius, xc, yc+radius);
}

void drawShowRotation (cairo_t *cr, double xc, double yc, double radius)
{
    cairo_set_font_face (cr,ft_default);
    cairo_set_font_size(cr, 18);
    cairo_set_line_width (cr, thickness);

    string text = "R";
    cairo_text_extents_t extents;
    cairo_text_extents (cr, text.c_str (), &extents);
    double tw = extents.width + 2.0*textPad;
    double th = extents.height + 2.0*textPad;

    double textradius = sqrt(tw*tw + th*th)/2.0;
    double scale = radius/textradius;
    double x = xc/scale - (extents.width/2.0 + extents.x_bearing);
    double y = yc/scale - (extents.height/2.0 + extents.y_bearing);
    cairo_scale (cr, scale, scale);

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_arc (cr, xc/scale, yc/scale, textradius, 0, 2.0*M_PI);
    cairo_stroke (cr);
    cairo_move_to (cr, x, y);
    cairo_show_text (cr, text.c_str ());
}

void drawX (cairo_t *cr, double xc, double yc, double radius)
{
    // Draw cross
    radius = radius*sqrt(2)/2;
    drawWire (cr,xc-radius, yc-radius, xc+radius, yc+radius);
    drawWire (cr,xc+radius, yc-radius, xc-radius, yc+radius);
}

void drawShowFred (cairo_t *cr, double width, double height)
{
    double Xrad = min(height/4.3, width/2.0);
    drawWire (cr, width/2, Xrad, width/2, height-Xrad);
    drawX (cr, width/2, Xrad, Xrad);
    drawX (cr, width/2, height-Xrad, Xrad);
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
            drawShowNOT (cr->cobj(), xc, yc, min(width/2.0, height/2.0));
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
