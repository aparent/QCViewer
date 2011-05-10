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

void draw (Circuit *c) {

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
