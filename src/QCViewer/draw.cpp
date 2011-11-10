#include "circuit.h"
#include <cairo.h>
#include <cairo-svg.h>
#include <cairo-ft.h>
#include <cmath>
#include <iostream>
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

void init_fonts(){
	FT_Init_FreeType( &library );
	FT_New_Face( library, "data/fonts/cmbx12.ttf", 0, &ft_face );
	ft_default = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
}

double wireToY (uint32_t x) {
  return yoffset+(x+1)*wireDist;
}

int pickWire (double y) {
  if (y < yoffset+wireDist/2) return -1;
  if (y < yoffset+wireDist) return 0;
  int wire = floor((y-yoffset)/wireDist - 1);
  if ((double)(y - wireToY (wire)) > wireDist/2) return wire + 1;
  return wire;
}


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

gateRect drawControls (cairo_t *cr, uint32_t xc, vector<Control> *ctrl, vector<uint32_t> *targ) {
  uint32_t minw, maxw;
  minmaxWire (ctrl, targ, &minw, &maxw);
  if (!ctrl->empty())drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw));
  for (uint32_t i = 0; i < ctrl->size(); i++) {
    drawDot (cr, xc, wireToY((*ctrl)[i].wire), dotradius, (*ctrl)[i].polarity);
  }
  gateRect rect;
  rect.x0 = xc-dotradius;
  rect.y0 = wireToY(minw)-dotradius;
  rect.width = 2*dotradius;
  rect.height = wireToY(maxw) - wireToY(minw) + 2*(dotradius);
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

gateRect drawCU (cairo_t *cr, uint32_t xc, string name, vector<Control> *ctrl, vector<uint32_t> *targ) {
  uint32_t minw, maxw;
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
  gateRect rect = drawControls (cr, xc-radius+width/2.0, ctrl, targ);
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
  cairo_show_text (cr, name.c_str());
  gateRect r;
  r.x0 = xc - thickness-radius;
  r.y0 = yc -height/2 - thickness;
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


gateRect drawCNOT (cairo_t *cr, uint32_t xc, vector<Control> *ctrl, vector<uint32_t> *targ) {
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  for (uint32_t i = 0; i < targ->size(); i++) {
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

gateRect drawFred (cairo_t *cr, uint32_t xc, vector<Control> *ctrl, vector<uint32_t> *targ) {
  gateRect rect = drawControls (cr, xc, ctrl, targ);
  uint32_t minw = (*targ)[0];
  uint32_t maxw = (*targ)[0];
  for (uint32_t i = 0; i < targ->size(); i++) {
    gateRect recttmp = drawX (cr, xc, wireToY((*targ)[i]), radius);
    rect = combine_gateRect(rect, recttmp);
    minw = min (minw, (*targ)[i]);
    maxw = max (maxw, (*targ)[i]);
  }
	if (ctrl->empty()) drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw));
  return rect;
}

void drawbase (cairo_t *cr, Circuit *c, double w, double h, double wirestart, double wireend) {
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_rectangle (cr, 0, 0, w, h); // TODO: document why the scale factors are here
  cairo_fill (cr);

  for (uint32_t i = 0; i < c->numLines(); i++) {
    double y = wireToY (i);
    drawWire (cr, wirestart+xoffset, y, wireend, y);
  }
}

int pickRect (vector<gateRect> rects, double x, double y) {
  for (int i = 0; i < (int)rects.size (); i++) {
    if (rects[i].x0 <= x && rects[i].x0+rects[i].width >= x && rects[i].y0 <= y && rects[i].y0 + rects[i].height >= y) return i;
  }
  return -1;
}

vector<uint32_t> pickRects (vector<gateRect> rects, gateRect s) {
  vector<uint32_t> ans;
  for (uint32_t i = 0; i < (uint32_t)rects.size (); i++) {
    if (rects[i].x0 <= s.x0 && rects[i].x0+rects[i].width <= s.x0) continue;
    if (s.x0 <= rects[i].x0 && s.x0+s.width <= rects[i].x0) continue;
    if (rects[i].y0 <= s.y0 && rects[i].y0+rects[i].height <= s.y0) continue;
    if (s.y0 <= rects[i].y0 && s.y0+s.height <= rects[i].y0) continue;
    ans.push_back (i);
  }
  return ans;
}

void drawloop(cairo_t* cr, Loop l, vector<gateRect> rects) {
  double red = 0;
  double green = 51.0/255.0;
  double blue = 102.0/255.0;
  double alpha1 = 0.7;
  double alpha2 = 0.2;
  
  gateRect r = rects[l.first];
  for (uint32_t i = l.first; i <= l.last; i++) {
    r = combine_gateRect (r, rects[i]);
  }

  cairo_set_source_rgba (cr, red, green, blue,alpha2);
  cairo_rectangle (cr, r.x0, r.y0, r.width, r.height);
  cairo_fill(cr);

  double dashes[] = { 4.0, 4.0 };
  cairo_set_dash (cr, dashes, 2, 0.0);
  cairo_set_line_width (cr, 2);
  cairo_set_source_rgba (cr, red, green, blue,alpha1);
  cairo_rectangle (cr, r.x0, r.y0, r.width, r.height);
  cairo_stroke (cr);

  stringstream ss;
  ss << l.label << " x ";
  cairo_set_font_size(cr, 16);
  cairo_text_extents_t extents, extents2;
  cairo_text_extents(cr, ss.str().c_str(), &extents);
  double x = r.x0;
  double y = r.y0 - (extents.height + extents.y_bearing) - 5.0;
  cairo_move_to(cr, x, y);
  cairo_show_text (cr, ss.str().c_str());
  stringstream ss2;
  ss2 << l.sim_n;
  cairo_set_font_size(cr, 22);
  cairo_text_extents(cr, ss2.str().c_str(), &extents2);
  x = r.x0 + extents.width + 3.0;
  y = r.y0 - (-extents2.height - extents2.y_bearing) - 5.0;
  cairo_move_to(cr, x, y);
  cairo_show_text (cr, ss2.str().c_str());
}

vector<gateRect> draw (cairo_t *cr, Circuit* c, vector<LayoutColumn>& columns, double *wirestart, double *wireend, bool forreal) {
  vector <gateRect> rects;
  cairo_set_source_rgb (cr, 0, 0, 0);

  // input labels
  double xinit = 0.0;
  for (uint32_t i = 0; i < c->numLines(); i++) {
    Line *line = c->getLine (i);
    string label = line->getInputLabel ();
    cairo_text_extents_t extents;
    cairo_text_extents(cr, label.c_str(), &extents);

    double x = 0, y = 0;
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
  uint32_t mingw, maxgw;
// TODO: remove  vector <int> parallels = c->getGreedyParallel ();

  uint32_t i = 0;
  double maxX;
  if (columns.empty()) cout << "WARNING: invalid layout detected in " << __FILE__ << " at line " << __LINE__ << "!\n";
  for (uint32_t j = 0; j < columns.size(); j++) {
    maxX = 0.0;
    for (; i <= columns[j].lastGateID; i++) {
      Gate* g = c->getGate (i);
      gateRect r;
      minmaxWire (&g->controls, &g->targets, &mingw, &maxgw);
      switch (g->drawType) {
        case Gate::NOT: r = drawCNOT (cr, xcurr, &g->controls, &g->targets); break;
        case Gate::FRED: r = drawFred (cr, xcurr, &g->controls, &g->targets); break;
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
          } else { ... } */
				  if (g->type == Gate::RGATE) {
				    string lbl = g->getName ();
						r = drawCU (cr, xcurr, lbl, &g->controls, &g->targets);
					} else {
            r = drawCU (cr, xcurr, g->getName(), &g->controls, &g->targets);
          }
					break;
       }
       rects.push_back(r);
       maxX = max (maxX, r.width);
    }
    xcurr += maxX;
    xcurr += gatePad + columns[j].pad;
  }
  xcurr -= gatePad;
  *wireend = xcurr;

  // output labels
  cairo_set_source_rgb (cr, 0, 0, 0);
  for (uint32_t i = 0; i < c->numLines (); i++) {
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


  // loops
  for (vector<Loop>::iterator it = c->loops.begin(); it != c->loops.end(); ++it) {
    drawloop (cr, *it, rects);
  }

  return rects;
}


void drawArchitectureWarnings (cairo_t* cr, vector<gateRect> rects, vector<int> badGates) {
  for (uint32_t i = 0; i < badGates.size(); i++) {
    drawRect (cr, rects[badGates[i]], Colour(0.8,0.1,0.1,0.7), Colour(0.8,0.4,0.4,0.3));
  }
}

void drawParallelSectionMarkings (cairo_t* cr, vector<gateRect> rects, int numLines, vector<int> pLines) {
  for (uint32_t i = 0; i < pLines.size() - 1; i++) {
    int gateNum = pLines[i];
    double x = (rects[gateNum].x0 + rects[gateNum].width + rects[gateNum+1].x0)/2;
    drawPWire (cr, x, numLines);
  }
}

void drawSelections (cairo_t* cr, vector<gateRect> rects, vector<uint32_t> selections) {
  //gateRect r = rects[selections[0]];
  for (uint32_t i = 0; i < (uint32_t)selections.size (); i++) {
    //r = combine_gateRect (r,rects[selections[i]]);
    drawRect(cr, rects[selections[i]], Colour (0.1, 0.2, 0.7, 0.7), Colour (0.1,0.2,0.7,0.3));
  }
  //drawRect (cr, r, Colour (0.1,0.2,0.7,0.7), Colour (0.1,0.2,0.7,0.3));
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


vector<gateRect> draw_circuit (Circuit *c, cairo_t* cr, vector<LayoutColumn>& columns, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, vector<uint32_t> selections) {
  cairo_scale (cr, scale, scale);
	cairo_set_font_face (cr,ft_default);
  cairo_set_font_size(cr, 18);

  vector<gateRect> rects;
  //Push the gate drawing into a group so that wireend can be determined and wires can be drawn first
	cairo_push_group (cr);
	rects = draw (cr, c, columns, &wirestart, &wireend, true);
	cairo_pattern_t *group = cairo_pop_group (cr);
  drawbase (cr, c, ext.width+ext.x, ext.height+ext.y+thickness, wirestart, wireend);
	cairo_set_source (cr, group);
	//Draw the gates
	cairo_paint(cr);
	cairo_pattern_destroy (group);
  if (drawParallel) drawParallelSectionMarkings (cr, rects, c->numLines(),c->getParallel());
  if (drawArch) drawArchitectureWarnings (cr, rects, c->getArchWarnings());
  if (!selections.empty()) drawSelections (cr, rects, selections);

  return rects;
}
