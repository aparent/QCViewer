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
                drawGate(g,cr,xcurr,maxX,rects);
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

void CircuitDrawer::drawGate(shared_ptr<Gate> g,cairo_t *cr,double &xcurr,double &maxX, std::vector <gateRect> &rects) const
{
    switch (g->type) {
    case Gate::UGATE:
        drawUGate (g,cr, xcurr,maxX,rects);
        break;
    case Gate::RGATE:
        drawUGate (g,cr, xcurr,maxX,rects);
        break;
    case Gate::SUBCIRC:
    default:
        drawSubcirc (static_pointer_cast<Subcircuit>(g),cr, xcurr,maxX,rects);
        break;
    }
}

void CircuitDrawer::drawUGate(shared_ptr<Gate> g,cairo_t *cr,double &xcurr,double &maxX, vector <gateRect> &rects) const
{
    gateRect r;
    switch (g->drawType) {
    case Gate::NOT:
        r = drawCNOT (g,cr, xcurr);
        break;
    case Gate::FRED:
        r = drawFred (g,cr, xcurr);
        break;
    case Gate::DEFAULT:
    default:
        r = drawCU (g,cr, xcurr);
        break;
    }
    rects.push_back(r);
    maxX = max (maxX, r.width);
}

gateRect CircuitDrawer::drawControls (shared_ptr<Gate> g,cairo_t *cr, const gateRect &r) const
{
    uint32_t center = r.x0+r.width/2.0;
    uint32_t top = r.y0;
    uint32_t bottem = r.y0+r.height;
    uint32_t mincw,maxcw,mintw,maxtw,minw,maxw;
    minmaxWire (g->controls, vector<unsigned int>() , mincw, maxcw);
    minmaxWire (vector<Control>(), g->targets, mintw, maxtw);
    minmaxWire (g->controls,g->targets, minw, maxw);
    if (!g->controls.empty()) {
        if (maxcw > maxtw) {
            drawWire (cr, center, wireToY (maxcw), center, bottem);
        }
        if (mincw < mintw) {
            drawWire (cr, center, wireToY (mincw), center, top);
        }
    }
    for (uint32_t i = 0; i < g->controls.size(); i++) {
        drawDot (cr, center, wireToY(g->controls.at(i).wire), dotradius, g->controls.at(i).polarity);
    }
    gateRect rect;
    rect.x0 = center-dotradius;
    rect.y0 = wireToY(minw)-dotradius;
    rect.width = 2*dotradius;
    rect.height = wireToY(maxw) - wireToY(minw) + 2*(dotradius);
    return rect;
}

gateRect CircuitDrawer::drawControls (shared_ptr<Gate> g,cairo_t *cr,uint32_t xc) const
{
    uint32_t mincw,maxcw,mintw,maxtw,minw,maxw;
    minmaxWire (g->controls, vector<unsigned int>() , mincw, maxcw);
    minmaxWire (vector<Control>(), g->targets, mintw, maxtw);
    minmaxWire (g->controls,g->targets, minw, maxw);
    if (!g->controls.empty()) {
        if (maxcw > maxtw) {
            drawWire (cr, xc, wireToY (maxcw), xc, wireToY (maxtw)+radius);
        }
        if (mincw < mintw) {
            drawWire (cr, xc, wireToY (mincw), xc, wireToY (mintw)-radius);
        }
    }
    for (uint32_t i = 0; i < g->controls.size(); i++) {
        drawDot (cr, xc, wireToY(g->controls.at(i).wire), dotradius, g->controls.at(i).polarity);
    }
    gateRect rect;
    rect.x0 = xc-dotradius;
    rect.y0 = wireToY(minw)-dotradius;
    rect.width = 2*dotradius;
    rect.height = wireToY(maxw) - wireToY(minw) + 2*(dotradius);
    return rect;
}

gateRect CircuitDrawer::drawFred (shared_ptr<Gate> g, cairo_t *cr, uint32_t xc) const
{
    gateRect rect = drawControls (g, cr, xc);
    uint32_t minw = g->targets.at(0);
    uint32_t maxw = g->targets.at(0);
    for (uint32_t i = 0; i < g->targets.size(); i++) {
        gateRect recttmp = drawX (cr, xc, wireToY(g->targets.at(i)), radius);
        rect = combine_gateRect(rect, recttmp);
        minw = min (minw, g->targets.at(i));
        maxw = max (maxw, g->targets.at(i));
    }
    if (!g->controls.empty()) {
        drawWire (cr, xc, wireToY (minw)-radius, xc, wireToY (maxw));
    } else {
        drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw));
    }
    return rect;
}

gateRect CircuitDrawer::drawCNOT (shared_ptr<Gate> g, cairo_t *cr, uint32_t xc) const
{
    gateRect rect = drawControls (g, cr, xc);
    for (uint32_t i = 0; i < g->targets.size(); i++) {
        gateRect recttmp = drawNOT (cr, xc, wireToY(g->targets.at(i)), radius);
        rect = combine_gateRect(rect, recttmp);
    }
    return rect;
}

gateRect CircuitDrawer::drawNOT (cairo_t *cr, double xc, double yc, double radius) const
{
    cairo_set_line_width (cr, thickness);
    // Draw black border
    cairo_move_to (cr, xc, yc);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
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

gateRect CircuitDrawer::drawX (cairo_t *cr, double xc, double yc, double radius) const
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

gateRect CircuitDrawer::drawCU (shared_ptr<Gate> g,cairo_t *cr, uint32_t xc) const
{
    uint32_t minw, maxw;
    stringstream ss;
    ss << g->getName();
    if (g->getLoopCount() > 1) {
        ss << " x" << g->getLoopCount();
    }
    vector<Control> dummy;
    minmaxWire (dummy, g->targets, minw, maxw); // only the targets
    // (XXX) need to do a  check in here re: target wires intermixed with not targets.

    double dw = wireToY(1)-wireToY(0);
    double yc = (wireToY (minw)+wireToY(maxw))/2;//-dw/2.0;
    double height = dw*(maxw-minw+Upad);

    // get width of this box
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, ss.str().c_str(), &extents);
    double width = extents.width+2*textPad;
    if (width < dw*Upad) {
        width = dw*Upad;
    }
    gateRect rect = drawControls (g, cr, xc-radius+width/2.0);
    cairo_rectangle (cr, xc-radius, yc-height/2, width, height);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);
    cairo_rectangle (cr, xc-radius, yc-height/2, width, height);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width (cr, thickness);
    cairo_stroke(cr);

    double x = (xc - radius + width/2) - extents.width/2 - extents.x_bearing;
    double y = yc - extents.height/2 - extents.y_bearing;
    cairo_move_to(cr, x, y);
    cairo_show_text (cr, ss.str().c_str());
    gateRect r;
    r.x0 = xc - thickness-radius;
    r.y0 = yc -height/2 - thickness;
    r.width = width + 2*thickness;
    r.height = height + 2*thickness;
    return combine_gateRect(rect, r);

}

void CircuitDrawer::drawSubcirc(shared_ptr<Subcircuit> s, cairo_t *cr,double &xcurr,double &maxX, vector <gateRect> &rects) const
{
    gateRect r;
    if (s->expand) {
        r = drawExp(s,cr,xcurr);
    } else {
        r = drawCU(static_pointer_cast<Gate>(s),cr,xcurr);
    }
    rects.push_back(r);
    maxX = max (maxX, r.width);
}

gateRect CircuitDrawer::drawExp(shared_ptr<Subcircuit> s,cairo_t *cr,double xcurr) const
{
    double maxX = 0.0;
    vector <gateRect> subRects;
    vector<unsigned int> para = s->getGreedyParallel();
    for(unsigned int j = 0; j <= (s->unroll * (s->getLoopCount()-1)); j++) {
        unsigned int currentCol = 0;
        for(unsigned int i = 0; i < s->numGates(); i++) {
            shared_ptr<Gate> g = s->getGate(i);
            drawGate(g,cr,xcurr,maxX,subRects);
            if(para.size() > currentCol && i == para[currentCol]) {
                xcurr += maxX - gatePad/2;
                if (s->circ->getGate(i)->breakpoint) {
                    unsigned int maxTarget = 0;
                    for (unsigned int i = 0; i < s->targets.size(); i++) {
                        if (s->targets.at(i) > maxTarget) {
                            maxTarget = s->targets.at(i);
                        }
                    }
                    cairo_set_source_rgba (cr,0.8,0,0,0.8);
                    cairo_move_to (cr,xcurr, wireToY(maxTarget+1));
                    cairo_line_to (cr,xcurr, wireToY(maxTarget - s->circ->numLines()));
                    cairo_stroke (cr);
                    cairo_set_source_rgb (cr, 0, 0, 0);
                }
                xcurr += gatePad*1.5;
                maxX = 0.0;
                currentCol++;
            }

            if (s->simState->simulating && !s->unroll && s->simState->gate == i + 1 ) {
                drawRect (cr, subRects.back(), Colour (0.1,0.7,0.2,0.7), Colour (0.1, 0.7,0.2,0.3));
            } else if (s->simState->simulating && s->unroll && s->simState->gate +(s->simState->loop-1)*s->numGates()  == j*s->numGates() + i + 1)  {
                drawRect (cr, subRects.back(), Colour (0.1,0.7,0.2,0.7), Colour (0.1, 0.7,0.2,0.3));
            }
        }
    }
    xcurr -= maxX;
    xcurr -= gatePad;
    gateRect r;
    if (subRects.size() > 0) {
        r = subRects.at(0);
        for (unsigned int i = 1; i < subRects.size(); i++) {
            //drawRect (cr, subRects->at(i), Colour(0.8,0,0,0.8), Colour (0.1, 0.7,0.2,0.3));
            r = combine_gateRect(r,(subRects)[i]);
        }
        drawSubCircBox(s,cr, r);
        r.subRects = subRects;
    }
    return r;
}

void CircuitDrawer::drawSubCircBox(shared_ptr<Subcircuit> s, cairo_t* cr, gateRect &r) const
{
    double dashes[] = { 4.0, 4.0 };
    cairo_set_dash (cr, dashes, 2, 0.0);
    cairo_set_line_width (cr, 2);
    cairo_rectangle (cr, r.x0, r.y0, r.width, r.height);
    cairo_stroke (cr);
    cairo_set_dash (cr, dashes, 0, 0.0);
    gateRect rect = drawControls (static_pointer_cast<Gate>(s), cr, r);
    stringstream ss;
    ss << s->getName();
    if (s->getLoopCount() > 1) {
        ss << " x" << s->getLoopCount();
        if (s->simState->simulating) {
            ss << " " << s->simState->loop << "/" << s->getLoopCount();
        }
    }
    cairo_text_extents_t extents;
    cairo_text_extents(cr, ss.str().c_str(), &extents);
    double x = r.x0;
    double y = r.y0 - (extents.height + extents.y_bearing) - 5.0;
    cairo_move_to(cr, x, y);
    cairo_show_text (cr, ss.str().c_str());
    r.height+=extents.height+10;
    r.y0-=extents.height+10;
    if (r.width < extents.width+4) {
        r.width = extents.width+4;
    }
    r = combine_gateRect(rect,r);
}
