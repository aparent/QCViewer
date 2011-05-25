#include "circuit.h"
#include <cairo.h>
#include <cmath>
#include <iostream>
#include <vector>

#define M_PI 3.141592

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

class gateRect {
public:
  float x0, y0;
	float width, height;
};

class Colour {
  public:
	  Colour () {}
		Colour (float rr, float gg, float bb, float aa) : r(rr), b(bb), g(gg), a(aa) {}
		float r, g, b, a;
};

float wireToY (int x) {
  return yoffset+(x+1)*wireDist;
}

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

void minmaxWire (vector<Control>* ctrl, vector<int>* targ, int *dstmin, int *dstmax) {
	int minw = (*targ)[0];
	int maxw = (*targ)[0];
	for (int i = 1; i < targ->size (); i++) {
    minw = min (minw, (*targ)[i]);
		maxw = max (maxw, (*targ)[i]);
	}

  for (int i = 0; i < ctrl->size (); i++) {
    minw = min (minw, (*ctrl)[i].wire);
		maxw = max (maxw, (*ctrl)[i].wire);
	}
	*dstmin = minw;
	*dstmax = maxw;
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
	for (int i = 0; i < ctrl->size(); i++) {
    drawDot (cr, xc, wireToY((*ctrl)[i].wire), dotradius, thickness, (*ctrl)[i].polarity);
  }
  if (ctrl->size() > 0)drawWire (cr, xc, wireToY (minw), xc, wireToY (maxw), thickness);
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


gateRect drawCNOT (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<int> *targ) {
	gateRect rect = drawControls (cr, xc, ctrl, targ);
	for (int i = 0; i < targ->size(); i++) {
    gateRect recttmp = drawNOT (cr, xc, wireToY((*targ)[i]), radius, thickness);
		rect = combine_gateRect(rect, recttmp);
  }
	return rect;
}

void drawbase (cairo_surface_t *surface, Circuit *c, float w, float h, float wirestart, float wireend, double scale) {
  cairo_t *cr = cairo_create (surface);
	cairo_scale (cr, scale, scale);
	cairo_set_source_surface (cr, surface, 0, 0);
	cairo_set_source_rgb (cr, 1, 1, 1);
	cairo_rectangle (cr, 0, 0, w/scale, h/scale);
	cairo_fill (cr);

  for (int i = 0; i < c->numLines(); i++) {
		float y = wireToY (i);
    drawWire (cr, wirestart, y, wireend, y, thickness);
	}
	cairo_destroy (cr);
}


vector<gateRect> draw (cairo_surface_t *surface, Circuit* c, double *wirestart, double *wireend, bool forreal, double scale) {
	vector <gateRect> rects;

	cairo_t *cr = cairo_create (surface);
	cairo_scale (cr, scale, scale);
	cairo_set_source_surface (cr, surface, 0.0, 0.0);

	cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, scale*18);
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
	int minw = -1;
	vector <int> parallels = c->getGreedyParallel ();

	// Draw them in parallel using the greedy strategy.
  int i = 0;
	for (int j = 0; j < parallels.size(); j++) {
		float maxwidth = 0;
    for (; i <= parallels[j]; i++) {
	    Gate* g = c->getGate (i);
	    gateRect r;
	    minmaxWire (&g->controls, &g->targets, &mingw, &maxgw);
			int count = 1; // hack
		  switch (g->gateType) {
        case NOT: r = drawCNOT (cr, xcurr, &g->controls, &g->targets); break;
		    default:
				  // XXX: this needs to be generalized!
					if (g->name.compare ("H") == 0) { // if hadamard
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
					} else {
				    r = drawCU (cr, xcurr, g->name, &g->controls, &g->targets);
					}
					break;
	 	  }
		  for (int i = 0; i < count; i++) rects.push_back(r);
			maxwidth = max (maxwidth, r.width);
		//  drawRect (cr, r, Colour (0.1,0.5,0.2,0.8), Colour (0.1, 0.5, 0.2, 0.3)); // DEBUG
	  }
    xcurr += maxwidth + gatePad;
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
  cairo_destroy (cr);

	return rects;
}


void drawArchitectureWarnings (cairo_surface_t* surface, vector<gateRect> rects, vector<int> badGates, double scale) {
	cairo_t *cr = cairo_create (surface);
	cairo_scale (cr, scale, scale);
	cairo_set_source_surface (cr, surface, 0.0, 0.0);
  for (int i = 0; i < badGates.size(); i++) {
    drawRect (cr, rects[badGates[i]], Colour(0.8,0.1,0.1,0.7), Colour(0.8,0.4,0.4,0.3));
	}
	cairo_destroy (cr);
}

void drawParallelSectionMarkings (cairo_surface_t* surface, vector<gateRect> rects, int numLines, vector<int> pLines, double scale) {
	cairo_t *cr = cairo_create (surface);
	cairo_scale (cr, scale, scale);
	cairo_set_source_surface (cr, surface, 0.0, 0.0);
	for (int i = 0; i < pLines.size() - 1; i++) {
		int gateNum = pLines[i];
		float x = (rects[gateNum].x0 + rects[gateNum].width + rects[gateNum+1].x0)/2;
    drawPWire (cr, x, numLines,thickness);
	}
  cairo_destroy (cr);
}

cairo_surface_t* make_png_surface (cairo_rectangle_t ext) {
  cairo_surface_t *img_surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, ext.width+ext.x, thickness+ext.height+ext.y);
  return img_surface;
}

cairo_rectangle_t get_circuit_size (Circuit *c, double* wirestart, double* wireend, double scale) {
	cairo_surface_t *unbounded_rec_surface = cairo_recording_surface_create (CAIRO_CONTENT_COLOR, NULL);
	draw (unbounded_rec_surface, c, wirestart, wireend, false, scale); // XXX fix up these inefficienies!!
	cairo_rectangle_t ext;
	cairo_recording_surface_ink_extents (unbounded_rec_surface, &ext.x, &ext.y, &ext.width, &ext.height);
  cairo_surface_destroy (unbounded_rec_surface);
	return ext;
}

void write_to_png (cairo_surface_t* surf, string filename) {
	cout << "writing... ";
	cairo_status_t status = cairo_surface_write_to_png (surf, filename.c_str());
  if (status != CAIRO_STATUS_SUCCESS) {
    cout << "Error saving to png." << endl;
		return;
	}
	cout << "success." << endl;
}

cairo_surface_t* makepicture (Circuit *c, bool drawArch, double scale) {
	vector<gateRect> rects;
	double wirestart, wireend;
  cairo_rectangle_t ext = get_circuit_size (c, &wirestart, &wireend, scale);
	cairo_surface_t* surf = make_png_surface (ext);
	if (surf == NULL) return NULL;
	drawbase (surf, c, ext.width+ext.x, ext.height+scale*ext.y+thickness, wirestart+xoffset, wireend, scale);
	rects = draw (surf, c, &wirestart, &wireend, true, scale);
  drawParallelSectionMarkings (surf, rects, c->numLines(),c->getParallel(), scale);
  if (drawArch) drawArchitectureWarnings (surf, rects, c->getArchWarnings(), scale);

  write_to_png (surf, "circuit.png");
	cairo_surface_destroy (surf);

	//////////////////(XXX)= c->getParallel();

}
