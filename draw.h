#ifndef DRAW__INCLUDED
#define DRAW__INCLUDED

#include <cairo.h>
#include <qc.h>
#include <string>

class gateRect {
public:
  float x0, y0;
  float width, height;
};

class Colour {
  public:
    Colour () {}
    Colour (float rr, float gg, float bb, float aa) : r(rr), g(gg), b(bb), a(aa) {}
    float r, g, b, a;
};

class LayoutColumn {
public:
  LayoutColumn (unsigned int, double);
  unsigned int lastGateID;
  double pad;
};

cairo_rectangle_t get_circuit_size (Circuit *c, vector<LayoutColumn>&, double* wirestart, double* wireend, double scale);
cairo_surface_t* make_png_surface (cairo_rectangle_t ext);
cairo_surface_t* make_svg_surface (std::string, cairo_rectangle_t);
vector<gateRect> draw_circuit (Circuit *c, cairo_t* cr, vector<LayoutColumn>&, bool drawArch, bool drawParallel, cairo_rectangle_t ext, double wirestart, double wireend, double scale, int);
void write_to_png (cairo_surface_t* surf, string filename);
int pickRect (vector<gateRect> rects, double x, double y);
void drawRect (cairo_t *cr, gateRect r, Colour outline, Colour fill);

#endif // DRAW__INCLUDED
