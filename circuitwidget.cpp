#include "circuitwidget.h"
#include <cairomm/context.h>
#include <circuit.h>
#include <circuitParser.h>
#include <iostream>
#include "draw.h"
#include <gtkmm.h>

using namespace std;

CircuitWidget::CircuitWidget() : circuit (NULL) {}
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
		cout << "Scale: " << scale << endl;
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->rectangle(event->area.x, event->area.y,
            event->area.width, event->area.height);
    cr->clip();
		if (circuit != NULL) draw_circuit (circuit, cr->cobj(), true, true,  ext, wirestart, wireend, scale);
  }
  return true;
}

void CircuitWidget::load (string file) {
	cout << "Opening " << file << endl;
	if (circuit != NULL) delete circuit;
  circuit = parseCircuit(file);
  if (circuit == NULL) {
    cout << "Error loading circuit" << endl;
		return;
	}
	ext = get_circuit_size (circuit, &wirestart, &wireend);
  win->resize (ext.width, yoffset+ext.height);

	cairo_surface_t* surface = make_png_surface (ext);
	cairo_t* cr = cairo_create (surface);
  cairo_set_source_surface (cr, surface, 0, 0);
	draw_circuit (circuit, cr, true, true,  ext, wirestart, wireend, 1.0);
	write_to_png (surface, "image.png");
}
