#ifndef DRAWINTERNAL__INCLUDED
#define DRAWINTERNAL__INCLUDED
#include "draw.h"

gateRect drawNOT (cairo_t *cr, double xc, double yc, double radius, bool opaque=true);
void drawShowRotation (cairo_t *cr, double xc, double yc, double radius);
void drawShowU (cairo_t *cr, double xc, double yc, double width, string name);
void drawShowFred (cairo_t *cr, double width, double height);

#endif
