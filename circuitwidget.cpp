#include "circuitwidget.h"
#include <cairomm/context.h>
#include <circuit.h>
#include <circuitParser.h>
#include <iostream>
#include "draw.h"
#include <gtkmm.h>

using namespace std;

CircuitWidget::CircuitWidget() : circuit (NULL), drawarch (false), drawparallel (false) {}
void CircuitWidget::set_window (Gtk::Window *w) { win = w; }
void CircuitWidget::set_offset (int y) { yoffset = y; }

CircuitWidget::~CircuitWidget () {}

bool CircuitWidget::on_expose_event(GdkEventExpose* event) {
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

//    double scale = min (width/ext.width, height/ext.height);
    double scale = width/ext.width;
    scale = 2.0;
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->rectangle(event->area.x, event->area.y,
                  event->area.width, event->area.height);
    cr->clip();
    if (circuit != NULL) draw_circuit (circuit, cr->cobj(), drawarch, drawparallel,  ext, wirestart, wireend, scale);
  }
  return true;
}

void CircuitWidget::load (string file) {
  if (circuit != NULL) delete circuit;
  circuit = parseCircuit(file);
  if (circuit == NULL) {
    cout << "Error loading circuit" << endl;
    return;
  }
  ext = get_circuit_size (circuit, &wirestart, &wireend, 2.0);
 // win->resize (ext.width, yoffset+ext.height);

 // cairo_surface_t* surface = make_png_surface (ext);
 // cairo_t* cr = cairo_create (surface);
 // cairo_set_source_surface (cr, surface, 0, 0);
 // draw_circuit (circuit, cr, true, true,  ext, wirestart, wireend, 1.0);
 // write_to_png (surface, "image.png");
}

void CircuitWidget::loadArch (string file) {
  if (circuit) circuit->parseArch (file);
}

void CircuitWidget::force_redraw () {
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                     get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}

void CircuitWidget::set_drawarch (bool foo) { drawarch = foo; force_redraw (); }
void CircuitWidget::set_drawparallel (bool foo) { drawparallel = foo; force_redraw (); }
