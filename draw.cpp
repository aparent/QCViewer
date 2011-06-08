#include "circuit.h"
#include <cairo.h>
#include <cairo-svg.h>
#include <cmath>
#include <iostream>
#include <vector>
#include "draw.h"

#ifndef M_PI
#define M_PI 3.141592
#endif

using namespace std;

// XXX organize this!!
float radius = 15.0;
float dotradius = 10.0;
float thickness = 2.0;
float xoffset = 10.0;
float yoffset = 10.0;
float wireDist = 40.0;
float gatePad = 18.0;
float textPad = 5.0;
float Upad = 0.9;


float wireToY (int x) {
  return yoffset+(x+1)*wireDist;
}

LayoutColumn::LayoutColumn (unsigned int g, double p) : lastGateID(g), pad(p) {}

void drawWire (cairo_t *cr, float x1, float y1, float x2, float y2, float thickness) {
  cairo_set_line_width (cr, thickness);
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_move_to (cr, x1, y1);
  cairo_line_to (cr, x2, y2);
  cairo_stroke (cr);
}

//for parallism wires
void drawPWire (cairo_t *cr, float x, int numLines, float thickness) {
  cairo_set_line_width (cr, thickness);
  cairo_set_source_rgba (cr, 0.4, 0.4, 0.4,0.4);
  cairo_move_to (cr, x, wireToY(0));
  cairo_line_to (cr, x, wireToY(numLines-1));
  cairo_stroke (cr);
  cairo_set_source_rgb (cr, 0, 0, 0);
}

void drawRect (cairo_t *cr, gateRect r, Colour outline, Colour fill) {
  cairo_set_source_rgba (cr, fill.r, fill.g, fill.b, fill.a);
  cairo_rectangle (cr, r.x0, r.y0, r.width, r.height);
  cairo_fill (cr);
  cairo_set_source_rgba (cr, outline.r, outline.g, outline.b, outline.a);
  cairo_rectangle (cr, r.x0, r.y0, r.width, r.height);
  cairo_stroke (cr);
}

gateRect combine_gateRect (gateRect a, gateRect b) {
  gateRect c;
  c.x0 = min(a.x0, b.x0);
  c.y0 = min(a.y0, b.y0);
  c.width =  max (a.x0-c.x0 + a.width,  b.x0 - c.x0 + b.width);
  c.height = max (a.y0-c.y0 + a.height, b.y0 - c.y0 + b.height);
  return c;
}


void drawDot (cairo_t *cr, float xc, float yc, float radius, float thickness, bool negative) {
  if (negative) {
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_fill (cr);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_set_line_width(cr, thickness);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_stroke (cr);
  } else {
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_fill (cr);
  }
}

gateRect drawControls (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<int> *targ) {
  int minw, maxw;
  minmaxWire (ctrl, targ, &minw, &maxw);
  if (ctrl->size() > 0)drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw), thickness);
  for (unsigned int i = 0; i < ctrl->size(); i++) {
    drawDot (cr, xc, wireToY((*ctrl)[i].wire), dotradius, thickness, (*ctrl)[i].polarity);
  }
  gateRect rect;
  rect.x0 = xc-dotradius;
  rect.y0 = wireToY(minw)-dotradius;
  rect.width = 2*dotradius;
  rect.height = wireToY(maxw) - wireToY(minw) + 2*(dotradius);
  // drawRect (cr, rect, Colour (0.1,0.2,0.6,0.8), Colour (0.1,0.2,0.6,0.3)); // DEBUG
  return rect;
}

gateRect drawCU (cairo_t *cr, unsigned int xc, string name, vector<Control> *ctrl, vector<int> *targ) {
  int minw, maxw;
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  vector<Control> dummy;
  minmaxWire (&dummy, targ, &minw, &maxw); // only the targets
  // (XXX) need to do a  check in here re: target wires intermixed with not targets.

  float dw = wireToY(1)-wireToY(0);
  float yc = (wireToY (minw)+wireToY(maxw))/2;//-dw/2.0;
  float height = dw*(maxw-minw+Upad);

  // get width of this box
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, name.c_str(), &extents);
  float width = extents.width+2*textPad;
  if (width < dw*Upad) {
    width = dw*Upad;
  }
  cairo_rectangle (cr, xc-width/2, yc-height/2, width, height);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_fill(cr);
  cairo_rectangle (cr, xc-width/2, yc-height/2, width, height);
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_set_line_width (cr, thickness);
  cairo_stroke(cr);

  float x = xc - (extents.width/2 + extents.x_bearing);
  float y = yc - (extents.height/2 + extents.y_bearing);
  cairo_move_to(cr, x, y);
  cairo_show_text (cr, name.c_str());
  gateRect r;
  r.x0 = xc - width/2 - thickness;
  r.y0 = yc - height/2 - thickness;
  r.width = width + 2*thickness;
  r.height = height + 2*thickness;
  return combine_gateRect(rect, r);
}

gateRect drawNOT (cairo_t *cr, float xc, float yc, float radius, float thickness) {
  // Draw white background
  cairo_set_line_width (cr, thickness);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
  cairo_fill (cr);

  // Draw black border
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
  //drawRect (cr, r, Colour (0.7,0.2,0.1,0.8), Colour(0.7,0.2,0.1,0.3)); DEBUG
  return r;
}

gateRect drawX (cairo_t *cr, float xc, float yc, float radius, float thickness) {
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


gateRect drawCNOT (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<int> *targ) {
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  for (unsigned int i = 0; i < targ->size(); i++) {
    gateRect recttmp = drawNOT (cr, xc, wireToY((*targ)[i]), radius, thickness);
    rect = combine_gateRect(rect, recttmp);
  }
  return rect;
}

gateRect drawFred (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<int> *targ) {
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  int minw = (*targ)[0];
  int maxw = (*targ)[0];
  for (unsigned int i = 0; i < targ->size(); i++) {
    gateRect recttmp = drawX (cr, xc, wireToY((*targ)[i]), radius, thickness);
    rect = combine_gateRect(rect, recttmp);
    minw = min (minw, (*targ)[i]);
    maxw = max (maxw, (*targ)[i]);
  }
	if (ctrl->size() == 0) drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw), thickness);
  return rect;
}

void drawbase (cairo_t *cr, Circuit *c, float w, float h, double wirestart, double wireend, double scale) {
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_rectangle (cr, 0, 0, w/scale, h/scale); // TODO: document why the scale factors are here
  cairo_fill (cr);

  for (int i = 0; i < c->numLines(); i++) {
    float y = wireToY (i);
    drawWire (cr, wirestart, y, wireend, y, thickness);
  }
}

int pickRect (vector<gateRect> rects, double x, double y) {
  for (int i = 0; i < (int)rects.size (); i++) {
    if (rects[i].x0 <= x && rects[i].x0+rects[i].width >= x && rects[i].y0 <= y && rects[i].y0 + rects[i].height >= y) return i;
  }
  return -1;
}

vector<gateRect> draw (cairo_t *cr, Circuit* c, vector<LayoutColumn>& columns, double *wirestart, double *wireend, bool forreal) {
  vector <gateRect> rects;
  cairo_set_source_rgb (cr, 0, 0, 0);

  // input labels
  double xinit = 0.0;
  for (int i = 0; i < c->numLines(); i++) {
    Line *line = c->getLine (i);
    string label = line->getInputLabel ();
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label.c_str(), &extents);

    double x, y;
    if (forreal) {
      x = *wirestart - extents.width;
      y = wireToY(i) - (extents.height/2 + extents.y_bearing);
    }
    cairo_move_to (cr, x, y);
    cairo_show_text (cr, label.c_str());
    xinit = max (xinit, extents.width);
  }

  if (!forreal) *wirestart = xinit;

  // gates
  float xcurr = xinit+2.0*gatePad;
  int mingw, maxgw;
// TODO ?? int minw = -1;
// TODO: remove  vector <int> parallels = c->getGreedyParallel ();

  // Draw them in parallel using the greedy strategy.
  unsigned int i = 0;
  cout << "columns size: " << columns.size() << endl << flush; 
  if (columns.size () == 0) cout << "WARNING: invalid layout detected in " << __FILE__ << " at line " << __LINE__ << "!\n";
  for (unsigned int j = 0; j < columns.size(); j++) {
    float maxwidth = 0;
    for (; i <= columns[j].lastGateID; i++) {
      Gate* g = c->getGate (i);
      gateRect r;
      minmaxWire (&g->controls, &g->targets, &mingw, &maxgw);
      int count = 1; // hack TODO: not nec. with the H-gate hack disabled
      switch (g->gateType) {
        case NOT: r = drawCNOT (cr, xcurr, &g->controls, &g->targets); break;
        case FRED: r = drawFred (cr, xcurr, &g->controls, &g->targets); break;
        default:
          // XXX: maybe expose as a setting? 
          /*if (g->name.compare ("H") == 0) { // if hadamard
            vector<Control> ctrl;
            vector<int> targ;
            do {
              count++;
              g = c->getGate (i);
              ctrl.insert(ctrl.end(), g->controls.begin(), g->controls.end());
              targ.insert(targ.end(), g->targets.begin(), g->targets.end());
              i++;
            } while (i <= parallels[j] && c->getGate(i)->name.compare("H") == 0);
            i--; // restore
            count--;
            // draw hadamards together. this isn't really cool. proof of concept.
            r = drawCU (cr, xcurr, g->name, &ctrl, &targ);
          } else {*/
            r = drawCU (cr, xcurr, g->name, &g->controls, &g->targets);
          //}
          break;
       }
       for (int i = 0; i < count; i++) rects.push_back(r);
       maxwidth = max (maxwidth, r.width);
    //  drawRect (cr, r, Colour (0.1,0.5,0.2,0.8), Colour (0.1, 0.5, 0.2, 0.3)); // DEBUG
    }
    xcurr += maxwidth + gatePad + columns[j].pad;
  }
  *wireend = xcurr - gatePad;

  // output labels
  cairo_set_source_rgb (cr, 0, 0, 0);
  for (int i = 0; i < c->numLines (); i++) {
    Line *line = c->getLine (i);
    string label = line->getOutputLabel();
    cairo_text_extents_t extents;
    cairo_text_extents (cr, label.c_str(), &extents);

    double x, y;
    x = *wireend + xoffset;
    y = wireToY(i) - (extents.height/2+extents.y_bearing);
    cairo_move_to (cr, x, y);
    cairo_show_text (cr, label.c_str());
  }

  return rects;
}


void drawArchitectureWarnings (cairo_t* cr, vector<gateRect> rects, vector<int> badGates) {
  for (unsigned int i = 0; i < badGates.size(); i++) {
    drawRect (cr, rects[badGates[i]], Colour(0.8,0.1,0.1,0.7), Colour(0.8,0.4,0.4,0.3));
  }
}

void drawParallelSectionMarkings (cairo_t* cr, vector<gateRect> rects, int numLines, vector<int> pLines) {
  for (unsigned int i = 0; i < pLines.size() - 1; i++) {
    int gateNum = pLines[i];
    float x = (rects[gateNum].x0 + rects[gateNum].width + rects[gateNum+1].x0)/2;
    drawPWire (cr, x, numLines,thickness);
  }
}

void drawSelections (cairo_t* cr, vector<gateRect> rects, int selection) {
  drawRect (cr, rects[selection], Colour (0.1,0.2,0.7,0.7), Colour (0.1,0.2,0.7,0.3));
}

cairo_surface_t* make_png_surface (cairo_rectangle_t ext) {
  cairo_surface_t *img_surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, ext.width+ext.x, thickness+ext.height+ext.y);
  return img_surface;
}

cairo_surface_t* make_svg_surface (string file, cairo_rectangle_t ext) {
  cairo_surface_t *img_surface = cairo_svg_surface_create (file.c_str(), ext.width+ext.x, thickness+ext.height+ext.y); // these measurements should be in points, w/e.
  return img_surface;
}

cairo_rectangle_t get_circuit_size (Circuit *c, vector<LayoutColumn>& columns, double* wirestart, double* wireend, double scale) {
cout << "hello\n" << flush;
  cairo_surface_t *unbounded_rec_surface = cairo_recording_surface_create (CAIRO_CONTENT_COLOR, NULL);
  cairo_t *cr = cairo_create(unbounded_rec_surface);
  cairo_set_source_surface (cr, unbounded_rec_surface, 0.0, 0.0);
  cairo_scale (cr, scale, scale);
  cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 18);
  draw (cr, c, columns, wirestart, wireend, false); // XXX fix up these inefficienies!!
  cairo_rectangle_t ext;
  cairo_recording_surface_ink_extents (unbounded_rec_surface, &ext.x, &ext.y, &ext.width, &ext.height);
  cairo_destroy (cr);
  cairo_surface_destroy (unbounded_rec_surface);
  return ext;
}

void write_to_png (cairo_surface_t* surf, string filename) {
  cairo_status_t status = cairo_surface_write_to_png (surf, filename.c_str());
  if (status != CAIRO_STATUS_SUCCESS) {
    cout << "Error saving to png." << endl;
    return;
  }
}


vector<gateRect> draw_circuit (Circuit *c, cairo_t* cr, vector<LayoutColumn>& columns, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, int selection) {
  cairo_scale (cr, scale, scale);
  cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 18);

  vector<gateRect> rects;
  drawbase (cr, c, ext.width+ext.x, ext.height+scale*ext.y+thickness, wirestart+xoffset, wireend, scale);
  rects = draw (cr, c, columns, &wirestart, &wireend, true);
  if (drawParallel) drawParallelSectionMarkings (cr, rects, c->numLines(),c->getParallel());
  if (drawArch) drawArchitectureWarnings (cr, rects, c->getArchWarnings());
  if (selection != -1) drawSelections (cr, rects, selection);
  return rects;
}
