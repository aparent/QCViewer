#ifndef DRAW__INCLUDED
#define DRAW__INCLUDED

#include <cairo.h>
#include <qc.h>
#include <string>
#include <vector>

class gateRect {
public:
	gateRect() : x0(0),y0(0),width(0),height(0){}
  double x0, y0;
  double width, height;
};

class Colour {
  public:
    Colour () : r(0.0),g(0.0),b(0.0),a(0.0) {}
    Colour (double rr, double gg, double bb, double aa) : r(rr), g(gg), b(bb), a(aa) {}
    double r, g, b, a;
};

class LayoutColumn {
public:
	LayoutColumn (uint32_t g, double p) : lastGateID(g), pad(p) {}
  uint32_t lastGateID;
  double pad;
  gateRect bounds;
};

void init_fonts();
cairo_rectangle_t get_circuit_size (Circuit *c, vector<LayoutColumn>&, double* wirestart, double* wireend, double scale);
cairo_surface_t* make_png_surface (cairo_rectangle_t ext);
cairo_surface_t* make_svg_surface (std::string, cairo_rectangle_t);
vector<gateRect> draw_circuit (Circuit *c, cairo_t* cr, vector<LayoutColumn>&, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, vector<uint32_t>);
void write_to_png (cairo_surface_t* surf, string filename);
int pickRect (vector<gateRect> rects, double x, double y);
vector<uint32_t> pickRects (vector<gateRect>, gateRect);
void drawRect (cairo_t *cr, gateRect r, Colour outline, Colour fill);
gateRect combine_gateRect (gateRect a, gateRect b);
int pickWire (double);
double wireToY (uint32_t);

#endif // DRAW__INCLUDED
