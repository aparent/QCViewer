#include <iostream>
#include <cairo-svg.h>
#include <cairo-ps.h>

#include "circuitDrawer.h"

using namespace std;

vector<gateRect> CircuitDrawer::draw (cairo_t* cr, Circuit &c, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, const vector<Selection> &selections, cairo_font_face_t * ft_default) const
{
    cairo_scale (cr, scale, scale);
    cairo_set_font_face (cr,ft_default);
    cairo_set_font_size(cr, 18);

    vector<gateRect> rects;
    //Push the gate drawing into a group so that wireend can be determined and wires can be drawn first
    cairo_push_group (cr);
    rects = drawCirc (cr, c, wirestart, wireend, true);
    cairo_pattern_t *group = cairo_pop_group (cr);
    drawbase (cr, c, ext.width+ext.x, ext.height+ext.y+thickness, wirestart, wireend);
    cairo_set_source (cr, group);
    //Draw the gates
    cairo_paint(cr);
    cairo_pattern_destroy (group);
    if (drawParallel) drawParallelSectionMarkings (cr, rects, c.numLines(), c.getParallel());
    if (drawArch) drawArchitectureWarnings (cr, rects, c.getArchWarnings());
    if (!selections.empty()) drawSelections (cr, rects, selections);

    return rects;
}

void CircuitDrawer::drawbase (cairo_t *cr, Circuit &c, double w, double h, double wirestart, double wireend) const
{
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_rectangle (cr, 0, 0, w, h); // TODO: document why the scale factors are here
    cairo_fill (cr);

    for (uint32_t i = 0; i < c.numLines(); i++) {
        double y = wireToY (i);
        drawWire (cr, wirestart+xoffset, y, wireend, y);
    }
}

vector<gateRect> CircuitDrawer::drawCirc (cairo_t *cr, Circuit &c, double &wirestart, double &wireend, bool forreal) const
{
    vector <gateRect> rects;
    cairo_set_source_rgb (cr, 0, 0, 0);

    // input labels
    double xinit = 0.0;
    for (uint32_t i = 0; i < c.numLines(); i++) {
        string label = c.getLine(i).getInputLabel();
        cairo_text_extents_t extents;
        cairo_text_extents(cr, label.c_str(), &extents);

        double x = 0, y = 0;
        if (forreal) {
            x = wirestart - extents.width;
            y = wireToY(i) - (extents.height/2 + extents.y_bearing);
        }
        cairo_move_to (cr, x, y);
        cairo_show_text (cr, label.c_str());
        xinit = max (xinit, extents.width);
    }

    if (!forreal) wirestart = xinit;

    // gates
    double xcurr = xinit+2.0*gatePad;
    uint32_t mingw, maxgw;
    unsigned int i = 0;
    double maxX = 0;

    if (c.numGates()>0) {
        for (uint32_t j = 0; j < c.columns.size(); j++) {
            maxX = 0.0;
            for (; i <= c.columns.at(j); i++) {
                shared_ptr<Gate> g = c.getGate (i);
                minmaxWire (g->controls, g->targets, mingw, maxgw);
                g->draw(cr,xcurr,maxX,rects);
                if (c.simState.simulating && c.simState.gate == i+1) {
                    drawRect (cr, rects.back(), Colour (0.1,0.7,0.2,0.7), Colour (0.1, 0.7,0.2,0.3));
                }
            }
            xcurr += maxX - gatePad/2;
            if (c.getGate(c.columns.at(j))->breakpoint) {
                cairo_set_source_rgba (cr,0.8,0,0,0.8);
                cairo_move_to (cr,xcurr, wireToY(-1));
                cairo_line_to (cr,xcurr, wireToY(c.numLines()));
                cairo_stroke (cr);
                cairo_set_source_rgb (cr, 0, 0, 0);
            }
            xcurr += gatePad*1.5;
        }
    }


    xcurr -= maxX;
    xcurr += gatePad;
    gateRect fullCirc;
    if (rects.size() > 0) {
        fullCirc = rects[0];
        for (unsigned int i = 1; i < rects.size(); i++) {
            fullCirc = combine_gateRect(fullCirc,rects[i]);
        }
    }
    wireend = wirestart + fullCirc.width + gatePad*2;

    // output labels
    cairo_set_source_rgb (cr, 0, 0, 0);
    for (uint32_t i = 0; i < c.numLines (); i++) {
        string label = c.getLine(i).getOutputLabel();
        cairo_text_extents_t extents;
        cairo_text_extents (cr, label.c_str(), &extents);

        double x, y;
        x = wireend + xoffset;
        y = wireToY(i) - (extents.height/2+extents.y_bearing);
        cairo_move_to (cr, x, y);
        cairo_show_text (cr, label.c_str());
    }
    return rects;
}

void CircuitDrawer::drawArchitectureWarnings (cairo_t* cr, const vector<gateRect> &rects, const vector<int> &badGates) const
{
    for (uint32_t i = 0; i < badGates.size(); i++) {
        drawRect (cr, rects[badGates[i]], Colour(0.8,0.1,0.1,0.7), Colour(0.8,0.4,0.4,0.3));
    }
}

void CircuitDrawer::drawParallelSectionMarkings (cairo_t* cr, const vector<gateRect> &rects, int numLines, const vector<int> &pLines) const
{
    for (uint32_t i = 0; i < pLines.size(); i++) {
        int gateNum = pLines[i];
        double x = (rects[gateNum].x0 + rects[gateNum].width + rects[gateNum+1].x0)/2;
        drawPWire (cr, x, numLines);
    }
}

void CircuitDrawer::drawPWire (cairo_t *cr, double x, int numLines) const
{
    cairo_set_line_width (cr, thickness);
    cairo_set_source_rgba (cr, 0.4, 0.4, 0.4,0.4);
    cairo_move_to (cr, x, wireToY(0));
    cairo_line_to (cr, x, wireToY(numLines-1));
    cairo_stroke (cr);
    cairo_set_source_rgb (cr, 0, 0, 0);
}

void CircuitDrawer::drawSelections (cairo_t* cr, const vector<gateRect> &rects, const vector<Selection> &selections) const
{
    for (unsigned int i = 0; i < selections.size (); i++) {
        if (selections[i].gate < rects.size()) { //XXX Why is this needed?
            drawRect(cr, rects[selections[i].gate], Colour (0.1, 0.2, 0.7, 0.7), Colour (0.1,0.2,0.7,0.3));
            if (!selections[i].sub.empty() && !rects[selections[i].gate].subRects.empty()) {
                drawSelections (cr, rects[selections[i].gate].subRects, selections[i].sub);
            }
        }
    }
}

cairo_rectangle_t CircuitDrawer::getCircuitSize (Circuit &c, double &wirestart, double &wireend, double scale, cairo_font_face_t * ft_default) const
{
    cairo_surface_t *unbounded_rec_surface = cairo_recording_surface_create (CAIRO_CONTENT_COLOR, NULL);
    cairo_t *cr = cairo_create(unbounded_rec_surface);
    cairo_set_source_surface (cr, unbounded_rec_surface, 0.0, 0.0);
    cairo_scale (cr, scale, scale);
    cairo_set_font_face (cr,ft_default);
    cairo_set_font_size(cr, 18);
    drawCirc (cr, c, wirestart, wireend, false); // XXX fix up these inefficienies!!
    cairo_rectangle_t ext;
    cairo_recording_surface_ink_extents (unbounded_rec_surface, &ext.x, &ext.y, &ext.width, &ext.height);
    cairo_destroy (cr);
    cairo_surface_destroy (unbounded_rec_surface);
    return ext;
}
///////
void CircuitDrawer::savepng (Circuit &c, string filename, cairo_font_face_t * ft_default)
{
    double wirestart, wireend;
    cairo_rectangle_t ext = getCircuitSize (c, wirestart, wireend, 1.0,ft_default);
    cairo_surface_t* surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, ext.width+ext.x, thickness+ext.height+ext.y);
    cairo_t* cr = cairo_create (surface);
    cairo_set_source_surface (cr, surface, 0, 0);
    draw( cr, c, false, false,  ext, wirestart, wireend, 1.0, vector<Selection>(), ft_default);
    write_to_png (surface, filename);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}

void CircuitDrawer::write_to_png (cairo_surface_t* surf, string filename) const
{
    cairo_status_t status = cairo_surface_write_to_png (surf, filename.c_str());
    if (status != CAIRO_STATUS_SUCCESS) {
        cout << "Error saving to png." << endl;
        return;
    }
}

void CircuitDrawer::savesvg (Circuit &c, string filename, cairo_font_face_t * ft_default)
{
    double wirestart, wireend;
    cairo_rectangle_t ext = getCircuitSize (c,wirestart, wireend, 1.0, ft_default);
    cairo_surface_t* surface = cairo_svg_surface_create (filename.c_str(), ext.width+ext.x, thickness+ext.height+ext.y);
    cairo_t* cr = cairo_create (surface);
    cairo_set_source_surface (cr, surface, 0, 0);
    draw(cr, c, false, false, ext, wirestart, wireend, 1.0, vector<Selection>(),ft_default);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}


void CircuitDrawer::saveps (Circuit &c, string filename,cairo_font_face_t * ft_default)
{
    double wirestart, wireend;
    cairo_rectangle_t ext = getCircuitSize (c,wirestart, wireend, 1.0,ft_default);
    cairo_surface_t* surface = make_ps_surface (filename, ext);
    cairo_t* cr = cairo_create(surface);
    cairo_set_source_surface (cr, surface, 0,0);
    draw(cr, c, false, false, ext, wirestart, wireend, 1.0, vector<Selection>(),ft_default);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
}

cairo_surface_t* CircuitDrawer::make_ps_surface (string file, cairo_rectangle_t ext) const
{
    cairo_surface_t *img_surface = cairo_ps_surface_create (file.c_str(), ext.width+ext.x, thickness+ext.height+ext.y);
    cairo_ps_surface_set_eps (img_surface, true);
    return img_surface;
}
