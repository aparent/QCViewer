#ifndef CIRCUITWIDGET__INCLUDED
#define CIRCUITWIDGET__INCLUDED

#include <gtkmm/drawingarea.h>
#include <circuit.h>
#include <string>

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
	int get_QCost ();
  int get_Depth ();
  int get_NumGates ();
  
protected:
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);

private:
  bool drawarch, drawparallel;
  Gtk::Window *win;
  int yoffset;
  Circuit *circuit;
  cairo_rectangle_t ext;
  double wirestart, wireend;
  void force_redraw ();
  double scale;
};

#endif
