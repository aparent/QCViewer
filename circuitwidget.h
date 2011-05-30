#ifndef CIRCUITWIDGET__INCLUDED
#define CIRCUITWIDGET__INCLUDED

#include <gtkmm/drawingarea.h>
#include <circuit.h>
#include <string>
#include "draw.h"

class CircuitWidget : public Gtk::DrawingArea {
public:
  CircuitWidget ();
  virtual ~CircuitWidget ();
  void load (string);
  void loadArch (string);

  void set_window (Gtk::Window *);
  void set_offset (int);
  void set_drawarch (bool);
  void set_drawparallel (bool);

  void savepng (std::string);
  void savesvg (std::string);

  void set_scale (double);
  double get_scale ();
  
protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_button_release_event(GdkEventButton* event);
  virtual bool onScrollEvent (GdkEventScroll* event);
  virtual bool onMotionEvent (GdkEventMotion* event);
  virtual bool on_button_press_event(GdkEventButton* event);

private:
  bool panning;
  double oldmousex, oldmousey;

  void toggle_selection (int);

  bool drawarch, drawparallel;
  Gtk::Window *win;
  int yoffset;
  Circuit *circuit;
  cairo_rectangle_t ext;
  double wirestart, wireend;
  void force_redraw ();
  double scale;
  double cx, cy;
  vector<gateRect> rects;
  int selection;
};

#endif
