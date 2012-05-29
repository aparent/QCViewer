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


#include <cairo.h>
#include <cairo-svg.h>
#include <cairo-ft.h>
#include <cairo-ps.h>
#include <cmath>
#include <iostream>

#include "QCLib/circuit.h"
#include "QCLib/subcircuit.h"
#include "draw.h"


#include <ft2build.h>

#ifndef M_PI
#define M_PI 3.141592
#endif

using namespace std;

//Font stuff
FT_Library library;
FT_Face ft_face;
cairo_font_face_t * ft_default;

// XXX organize this!!
const double radius = 15.0;
const double dotradius = 10.0;
const double thickness = 2.0;
const double xoffset = 10.0;
const double yoffset = 10.0;
const double wireDist = 40.0;
const double gatePad = 18.0;
const double textPad = 5.0;
const double Upad = 0.9;

void init_fonts()
{
    FT_Init_FreeType( &library );
    FT_New_Face( library, "data/fonts/cmbx12.ttf", 0, &ft_face );
    ft_default = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
}

int pickWire (double y)
{
    if (y < yoffset+wireDist/2) return -1;
    if (y < yoffset+wireDist) return 0;
    int wire = floor((y-yoffset)/wireDist - 1);
    if ((double)(y - wireToY (wire)) > wireDist/2) return wire + 1;
    return wire;
}

//for parallism wires
void drawPWire (cairo_t *cr, double x, int numLines)
{
    cairo_set_line_width (cr, thickness);
    cairo_set_source_rgba (cr, 0.4, 0.4, 0.4,0.4);
    cairo_move_to (cr, x, wireToY(0));
    cairo_line_to (cr, x, wireToY(numLines-1));
    cairo_stroke (cr);
    cairo_set_source_rgb (cr, 0, 0, 0);
}

gateRect drawControls (cairo_t *cr, uint32_t xc, const vector<Control> &ctrl, const vector<uint32_t> &targ)
{
    uint32_t minw, maxw;
    minmaxWire (ctrl, targ, minw, maxw);
    if (!ctrl.empty())drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw));
    for (uint32_t i = 0; i < ctrl.size(); i++) {
        drawDot (cr, xc, wireToY(ctrl[i].wire), dotradius, ctrl[i].polarity);
    }
    gateRect rect;
    rect.x0 = xc-dotradius;
    rect.y0 = wireToY(minw)-dotradius;
    rect.width = 2*dotradius;
    rect.height = wireToY(maxw) - wireToY(minw) + 2*(dotradius);
    return rect;
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

gateRect drawNOT (cairo_t *cr, double xc, double yc, double radius)
{
    cairo_set_line_width (cr, thickness);
    // Draw black border
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_stroke (cr);

    // Draw cross
    cairo_move_to (cr, xc-radius, yc);
    cairo_line_to (cr, xc+radius, yc);
    cairo_stroke (cr);
    cairo_move_to (cr, xc, yc-radius);
    cairo_line_to (cr, xc, yc+radius);
    cairo_stroke (cr);

    gateRect r;
    r.x0 = xc-radius-thickness;
    r.y0 = yc-radius-thickness;
    r.width = 2*(radius+thickness);
    r.height = r.width;
    return r;
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

gateRect drawX (cairo_t *cr, double xc, double yc, double radius)
{
    // Draw cross
    radius = radius*sqrt(2)/2;
    cairo_move_to (cr, xc-radius, yc-radius);
    cairo_line_to (cr, xc+radius, yc+radius);
    cairo_stroke (cr);
    cairo_move_to (cr, xc+radius, yc-radius);
    cairo_line_to (cr, xc-radius, yc+radius);
    cairo_stroke (cr);

    gateRect r;
    r.x0 = xc-radius-thickness;
    r.y0 = yc-radius-thickness;
    r.width = 2*(radius+thickness);
    r.height = r.width;
    return r;
}

void drawShowFred (cairo_t *cr, double width, double height)
{
    double Xrad = min(height/4.3, width/2.0);
    drawWire (cr, width/2, Xrad, width/2, height-Xrad);
    drawX (cr, width/2, Xrad, Xrad);
    drawX (cr, width/2, height-Xrad, Xrad);
}

void drawbase (cairo_t *cr, Circuit *c, double w, double h, double wirestart, double wireend)
{
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_rectangle (cr, 0, 0, w, h); // TODO: document why the scale factors are here
    cairo_fill (cr);

    for (uint32_t i = 0; i < c->numLines(); i++) {
        double y = wireToY (i);
        drawWire (cr, wirestart+xoffset, y, wireend, y);
    }
}

int pickRect (const vector<gateRect> &rects, double x, double y, vector<int> &selections)
{
    for (int i = 0; i < (int)rects.size (); i++) {
        if (rects[i].x0 <= x && rects[i].x0+rects[i].width >= x && rects[i].y0 <= y && rects[i].y0 + rects[i].height >= y) selections.push_back(i);
        if (!rects[i].subRects.empty()) {
            pickRect (rects[i].subRects,x,y,selections);
        }
    }
    if (selections.size() > 0) {
        return selections.at(0);
    }
    return -1;
}

vector<Selection> pickRects (const vector<gateRect> &rects, const gateRect &s)
{
    vector<Selection> ans;
    for (uint32_t i = 0; i < (uint32_t)rects.size (); i++) {
        if (rects[i].x0 <= s.x0 && rects[i].x0+rects[i].width <= s.x0) continue;
        if (s.x0 <= rects[i].x0 && s.x0+s.width <= rects[i].x0) continue;
        if (rects[i].y0 <= s.y0 && rects[i].y0+rects[i].height <= s.y0) continue;
        if (s.y0 <= rects[i].y0 && s.y0+s.height <= rects[i].y0) continue;
        if (!rects[i].subRects.empty()) {
            vector<Selection> sub;
            sub = pickRects (rects[i].subRects, s);
            ans.push_back (Selection(i,sub));
        } else {
            ans.push_back (Selection(i));
        }
    }
    return ans;
}

cairo_surface_t* make_png_surface (cairo_rectangle_t ext)
{
    cairo_surface_t *img_surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, ext.width+ext.x, thickness+ext.height+ext.y);
    return img_surface;
}

cairo_surface_t* make_svg_surface (string file, cairo_rectangle_t ext)
{
    cairo_surface_t *img_surface = cairo_svg_surface_create (file.c_str(), ext.width+ext.x, thickness+ext.height+ext.y); // these measurements should be in points, w/e.
    return img_surface;
}

cairo_surface_t* make_ps_surface (string file, cairo_rectangle_t ext)
{
    cairo_surface_t *img_surface = cairo_ps_surface_create (file.c_str(), ext.width+ext.x, thickness+ext.height+ext.y);
    cairo_ps_surface_set_eps (img_surface, true);
    return img_surface;
}

void write_to_png (cairo_surface_t* surf, string filename)
{
    cairo_status_t status = cairo_surface_write_to_png (surf, filename.c_str());
    if (status != CAIRO_STATUS_SUCCESS) {
        cout << "Error saving to png." << endl;
        return;
    }
}

