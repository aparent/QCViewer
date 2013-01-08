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
#include <cairo-ft.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.141592
#endif


#define G_BUTTON_SIZE 25

#define LINE_THICKNESS 2.0
#define TEXT_PAD 5.0

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

void GateIcon::drawWire (cairo_t *cr, double x1, double y1, double x2, double y2)
{
    cairo_set_line_width (cr, LINE_THICKNESS);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_move_to (cr, x1, y1);
    cairo_line_to (cr, x2, y2);
    cairo_stroke (cr);
}

void GateIcon::drawShowU (cairo_t *cr, double xc, double yc, double width, string name)
{
    double w,h;
    TextObject* text = textEngine.renderText(name);
    w = text->getWidth();
    h = text->getHeight();
    cairo_rectangle (cr, xc - width/2.0, yc- width/2.0, width, width);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_stroke (cr);
    double scale = width/(max(w,h)+TEXT_PAD);
    cairo_scale(cr,scale,scale);
    double x = (1.0/scale)*(xc) - (1.0/2.0)*w;
    double y = (1.0/scale)*(yc)- (1.0/2.0)*h ;
    text->draw(cr,x,y);
}

void GateIcon::drawShowMeasure (cairo_t *cr, double xc, double yc, double width)
{
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_move_to (cr, xc-width/2, yc);
    cairo_line_to (cr, xc+width/2, yc+width/2);
    cairo_line_to (cr, xc+width/2, yc-width/2);
    cairo_line_to (cr, xc-width/2, yc);
    cairo_stroke (cr);
}

void GateIcon::drawShowNOT (cairo_t *cr, double xc, double yc, double radius)
{
    cairo_set_line_width (cr, LINE_THICKNESS);
    // Draw black border
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_stroke (cr);

    // Draw cross
    drawWire (cr,xc-radius, yc, xc+radius, yc);
    drawWire (cr,xc, yc-radius, xc, yc+radius);
}

void GateIcon::drawShowRotation (cairo_t *cr, double xc, double yc, double radius)
{
    double w,h;
    TextObject* text = textEngine.renderText("R");
    w = text->getWidth();
    h = text->getHeight();
    double tw = w + TEXT_PAD;
    double th = h + TEXT_PAD;

    double textradius = sqrt(tw*tw + th*th)/2.0;
    double scale = radius/textradius;
    double x = xc/scale - w/2.0;
    double y = yc/scale - h/2.0;
    cairo_scale (cr, scale, scale);

    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_arc (cr, xc/scale, yc/scale, textradius, 0, 2.0*M_PI);
    cairo_stroke (cr);
    text->draw(cr,x,y);
}

void GateIcon::drawX (cairo_t *cr, double xc, double yc, double radius)
{
    // Draw cross
    radius = radius*sqrt(2)/2;
    drawWire (cr,xc-radius, yc-radius, xc+radius, yc+radius);
    drawWire (cr,xc+radius, yc-radius, xc-radius, yc+radius);
}

void GateIcon::drawShowFred (cairo_t *cr, double width, double height)
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
        case MEASURE:
            drawShowMeasure (cr->cobj (), xc, yc, min(width, height));
            break;
        case DEFAULT:
            drawShowU (cr->cobj (), xc, yc, min(width, height), dname);
        }
    }
    return true;
}
