#include "circuit.h"
#include <cairo.h>
#include <cairo-svg.h>
#include <cairo-ft.h>
#include <cmath>
#include <iostream>
#include <vector>
#include "draw.h"


#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef M_PI
#define M_PI 3.141592
#endif

using namespace std;

//Font stuff
FT_Library library;
FT_Face ft_face;
cairo_font_face_t * ft_default;

// XXX organize this!!
double radius = 15.0;
double dotradius = 10.0;
double thickness = 2.0;
double xoffset = 10.0;
double yoffset = 10.0;
double wireDist = 40.0;
double gatePad = 18.0;
double textPad = 5.0;
double Upad = 0.9;

void init_fonts(){
	FT_Init_FreeType( &library );
	FT_New_Face( library, "fonts/cmbx12.ttf", 0, &ft_face );
	ft_default = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
}

double wireToY (unsigned int x) {
  return yoffset+(x+1)*wireDist;
}

int pickWire (double y) {
  if (y < yoffset+wireDist/2) return -1;
  if (y < yoffset+wireDist) return 0;
  int wire = floor((y-yoffset)/wireDist - 1);
  if ((double)(y - wireToY (wire)) > wireDist/2) return wire + 1;
  return wire;
}

LayoutColumn::LayoutColumn (unsigned int g, double p) : lastGateID(g), pad(p) {}

void drawWire (cairo_t *cr, double x1, double y1, double x2, double y2) {
  cairo_set_line_width (cr, thickness);
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_move_to (cr, x1, y1);
  cairo_line_to (cr, x2, y2);
  cairo_stroke (cr);
}

//for parallism wires
void drawPWire (cairo_t *cr, double x, int numLines) {
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


void drawDot (cairo_t *cr, double xc, double yc, double radius, bool negative) {
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

gateRect drawControls (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<unsigned int> *targ) {
  unsigned int minw, maxw;
  minmaxWire (ctrl, targ, &minw, &maxw);
  if (ctrl->size() > 0)drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw));
  for (unsigned int i = 0; i < ctrl->size(); i++) {
    drawDot (cr, xc, wireToY((*ctrl)[i].wire), dotradius, (*ctrl)[i].polarity);
  }
  gateRect rect;
  rect.x0 = xc-dotradius;
  rect.y0 = wireToY(minw)-dotradius;
  rect.width = 2*dotradius;
  rect.height = wireToY(maxw) - wireToY(minw) + 2*(dotradius);
  // drawRect (cr, rect, Colour (0.1,0.2,0.6,0.8), Colour (0.1,0.2,0.6,0.3)); // DEBUG
  return rect;
}

void drawShowU (cairo_t *cr, double xc, double yc, double width, string name) {
	cairo_set_font_face (cr,ft_default);
  cairo_set_font_size(cr, 18);
  cairo_text_extents_t extents;
	cairo_text_extents (cr, name.c_str (), &extents);
  cairo_rectangle (cr, xc - width/2.0, yc- width/2.0, width, width);
	cairo_set_source_rgb (cr, 0, 0, 0);
	cairo_stroke (cr);
	double scale = width/(max(extents.width,extents.height)+textPad*2);
	cairo_scale(cr,scale,scale);
	double x = (1.0/scale)*(xc) - (1.0/2.0)*extents.width;
	double y = (1.0/scale)*(yc) - (1.0/2.0)*extents.y_bearing;
	cairo_move_to (cr, x, y);
	cairo_show_text (cr, name.c_str());
}

gateRect drawCU (cairo_t *cr, unsigned int xc, string name, vector<Control> *ctrl, vector<unsigned int> *targ) {
  unsigned int minw, maxw;
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  vector<Control> dummy;
  minmaxWire (&dummy, targ, &minw, &maxw); // only the targets
  // (XXX) need to do a  check in here re: target wires intermixed with not targets.

  double dw = wireToY(1)-wireToY(0);
  double yc = (wireToY (minw)+wireToY(maxw))/2;//-dw/2.0;
  double height = dw*(maxw-minw+Upad);

  // get width of this box
  cairo_set_source_rgb (cr, 0, 0, 0);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, name.c_str(), &extents);
  double width = extents.width+2*textPad;
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

  double x = xc - (extents.width/2 + extents.x_bearing);
  double y = yc - (extents.height/2 + extents.y_bearing);
  cairo_move_to(cr, x, y);
  cairo_show_text (cr, name.c_str());
  gateRect r;
  r.x0 = xc - width/2 - thickness;
  r.y0 = yc - height/2 - thickness;
  r.width = width + 2*thickness;
  r.height = height + 2*thickness;
  return combine_gateRect(rect, r);
}

gateRect drawNOT (cairo_t *cr, double xc, double yc, double radius, bool opaque=true) {
  cairo_set_line_width (cr, thickness);
  // Draw white background
	if (opaque) {
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_fill (cr);
  }
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
  return r;
}

void drawShowRotation (cairo_t *cr, double xc, double yc, double radius) {
  //cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_face (cr,ft_default);
  cairo_set_font_size(cr, 18);
  cairo_set_line_width (cr, thickness);

  string text = "R";
  cairo_text_extents_t extents;
	cairo_text_extents (cr, text.c_str (), &extents);
//	double textwidth = extents.width+2.0*textPad;
//	double textheight = extents.height+2.0*textPad;
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

gateRect drawX (cairo_t *cr, double xc, double yc, double radius) {
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


gateRect drawCNOT (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<unsigned int> *targ) {
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  for (unsigned int i = 0; i < targ->size(); i++) {
    gateRect recttmp = drawNOT (cr, xc, wireToY((*targ)[i]), radius);
    rect = combine_gateRect(rect, recttmp);
  }
  return rect;
}

void drawShowFred (cairo_t *cr, double width, double height) {
  double Xrad = min(height/4.3, width/2.0);
	drawWire (cr, width/2, Xrad, width/2, height-Xrad);
  drawX (cr, width/2, Xrad, Xrad);
	drawX (cr, width/2, height-Xrad, Xrad);
}

gateRect drawFred (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<unsigned int> *targ) {
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  unsigned int minw = (*targ)[0];
  unsigned int maxw = (*targ)[0];
  for (unsigned int i = 0; i < targ->size(); i++) {
    gateRect recttmp = drawX (cr, xc, wireToY((*targ)[i]), radius);
    rect = combine_gateRect(rect, recttmp);
    minw = min (minw, (*targ)[i]);
    maxw = max (maxw, (*targ)[i]);
  }
	if (ctrl->size() == 0) drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw));
  return rect;
}

void drawbase (cairo_t *cr, Circuit *c, double w, double h, double wirestart, double wireend, double scale) {
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_rectangle (cr, 0, 0, w/scale, h/scale); // TODO: document why the scale factors are here
  cairo_fill (cr);

  for (unsigned int i = 0; i < c->numLines(); i++) {
    double y = wireToY (i);
    drawWire (cr, wirestart, y, wireend, y);
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
  for (unsigned int i = 0; i < c->numLines(); i++) {
    Line *line = c->getLine (i);
    string label = line->getInputLabel ();
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label.c_str(), &extents);

    double x = 0, y = 0; // XXX: these were not initialized - why didn't this cause a bug?
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
  double xcurr = xinit+2.0*gatePad;
  unsigned int mingw, maxgw;
// TODO: remove  vector <int> parallels = c->getGreedyParallel ();

  // Draw them in parallel using the greedy strategy.
  unsigned int i = 0;
  if (columns.size () == 0) cout << "WARNING: invalid layout detected in " << __FILE__ << " at line " << __LINE__ << "!\n";
  double maxwidth = 0.0;
  for (unsigned int j = 0; j < columns.size(); j++) {
		xcurr += maxwidth;
		maxwidth = 0.0;
    for (; i <= columns[j].lastGateID; i++) {
      Gate* g = c->getGate (i);
      gateRect r;
      minmaxWire (&g->controls, &g->targets, &mingw, &maxgw);
      int count = 1; // XXX: hack for clumping hadamards, remove soon
      switch (g->drawType) {
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
            r = drawCU (cr, xcurr, g->getName(), &g->controls, &g->targets);
          //}
          break;
       }
       for (int i = 0; i < count; i++) rects.push_back(r);
       maxwidth = max (maxwidth, r.width);
    //  drawRect (cr, r, Colour (0.1,0.5,0.2,0.8), Colour (0.1, 0.5, 0.2, 0.3)); // DEBUG
    }
    xcurr += gatePad + columns[j].pad;
  }
  *wireend = xcurr+gatePad;

  // output labels
  cairo_set_source_rgb (cr, 0, 0, 0);
  for (unsigned int i = 0; i < c->numLines (); i++) {
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
    double x = (rects[gateNum].x0 + rects[gateNum].width + rects[gateNum+1].x0)/2;
    drawPWire (cr, x, numLines);
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
  cairo_surface_t *unbounded_rec_surface = cairo_recording_surface_create (CAIRO_CONTENT_COLOR, NULL);
  cairo_t *cr = cairo_create(unbounded_rec_surface);
  cairo_set_source_surface (cr, unbounded_rec_surface, 0.0, 0.0);
  cairo_scale (cr, scale, scale);
  //cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_face (cr,ft_default);
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
	cairo_set_font_face (cr,ft_default);
  //cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 18);

  vector<gateRect> rects;
  drawbase (cr, c, ext.width+ext.x, ext.height+scale*ext.y+thickness, wirestart+xoffset, wireend, scale);
  rects = draw (cr, c, columns, &wirestart, &wireend, true);
  if (drawParallel) drawParallelSectionMarkings (cr, rects, c->numLines(),c->getParallel());
  if (drawArch) drawArchitectureWarnings (cr, rects, c->getArchWarnings());
  if (selection != -1) drawSelections (cr, rects, selection);
  return rects;
}
