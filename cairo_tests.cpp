#include <cairo.h>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

class Control {
public:
  Control (int w, bool p) : wire(w), polarity(p) {}
  int wire;
  bool polarity;
};

void drawDot (cairo_t *cr, float xc, float yc, float radius, float thickness, bool negative) {
  if (negative) {
    cairo_set_source_rgba (cr, 1, 1, 1, 1);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_fill (cr);
    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    cairo_set_line_width(cr, thickness);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_stroke (cr);
  } else {
    cairo_set_source_rgba (cr, 0, 0, 0, 1);
    cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
    cairo_fill (cr);
  }
}

float radius = 15.0;
float dotradius = 5.0;
float thickness = 2.0;
float xoffset = 10.0;
float yoffset = 10.0;
float wireDist = 40.0;

float wireToY (int x) {
  return yoffset+(x+1)*wireDist;
}


void drawBox (cairo_t *cr, string name, float xc, float yc, float height, float thickness, float minpad) {

  // get width of this box
  cairo_select_font_face(cr, "Courier", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 35);
  cairo_set_source_rgba (cr, 0, 0, 0, 1);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, name.c_str(), &extents);

  cairo_rectangle (cr, xc-extents.width/2-minpad, yc-(height+0*extents.height)/2, extents.width+2*minpad, height);
  cairo_set_source_rgba (cr, 1, 1, 1, 1);
  cairo_fill(cr);
  cairo_rectangle (cr, xc-extents.width/2-minpad, yc-(height+0*extents.height)/2, extents.width+2*minpad, height);
  cairo_set_source_rgba (cr, 0, 0, 0, 1);
  cairo_set_line_width (cr, thickness);
  cairo_stroke(cr);

  float x = xc - (extents.width/2 + extents.x_bearing);
  float y = yc - (extents.height/2 + extents.y_bearing);
  cairo_move_to(cr, x, y);
  cairo_show_text (cr, name.c_str());
}

void drawNOT (cairo_t *cr, float xc, float yc, float radius, float thickness) {
  // Draw white background
  cairo_set_line_width (cr, thickness);
  cairo_set_source_rgba (cr, 1, 1, 1, 1);
  cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
  cairo_fill (cr);

  // Draw black border
  cairo_set_source_rgba (cr, 0,0,0,1);
  cairo_arc (cr, xc, yc, radius, 0, 2*M_PI);
  cairo_stroke (cr);

  // Draw cross
  cairo_move_to (cr, xc-radius, yc);
  cairo_line_to (cr, xc+radius, yc);
  cairo_stroke (cr);
  cairo_move_to (cr, xc, yc-radius);
  cairo_line_to (cr, xc, yc+radius);
  cairo_stroke (cr);
}

void drawWire (cairo_t *cr, float x1, float y1, float x2, float y2, float thickness) {
  cairo_set_line_width (cr, thickness);
  cairo_set_source_rgba (cr, 0, 0, 0, 1);
  cairo_move_to (cr, x1, y1);
  cairo_line_to (cr, x2, y2);
  cairo_stroke (cr);
}

void drawCNOT (cairo_t *cr, unsigned int xc, vector<Control> *ctrl, vector<int> *targ) {
  drawWire (cr, xc, wireToY (min((*targ)[0], (*ctrl)[0].wire)),
                xc, wireToY (max((*targ)[targ->size()-1], (*ctrl)[ctrl->size()-1].wire)),
                thickness);
  for (int i = 0; i < ctrl->size(); i++) {
    drawDot (cr, xc, wireToY((*ctrl)[i].wire), dotradius, thickness, (*ctrl)[i].polarity);
  }
  for (int i = 0; i < targ->size(); i++) {
    drawNOT (cr, xc, wireToY((*targ)[i]), radius, thickness);
  }
}

int main (int argc, char *argv[]) {
  cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 640, 480);
  cairo_t *cr = cairo_create (surface);
  cairo_rectangle (cr, 0,0,640,480);
  cairo_set_source_rgba (cr, 1,1,1,1);
  cairo_fill(cr);

  int numwires = 7;
  for (int i = 0; i < numwires; i++) {
    float y = wireToY (i);
    drawWire (cr, xoffset, y, 640, y, thickness);
  }

  vector<int> targ;
  vector<Control> ctrl;
  targ.push_back(3);
  ctrl.push_back(Control(0, false));
  ctrl.push_back(Control(1, true));
  drawCNOT (cr, 30, &ctrl, &targ);
  //draw_Box (cr, "QFT", 300,175,100,2,5);
  cairo_destroy (cr);
  cairo_surface_write_to_png (surface, "hello.png");
  cairo_surface_destroy (surface);
  return 0;
}
