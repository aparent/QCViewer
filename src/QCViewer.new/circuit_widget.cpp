#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>
#include "circuit_widget.h"

circuit_widget_t::circuit_widget_t ()
{
  circuit = NULL;
}

circuit_widget_t::~circuit_widget_t ()
{
  delete circuit;
}

bool circuit_widget_t::on_expose_event (GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::Window> window = get_window();
  if (!window) return true;

  Gtk::Allocation allocation = get_allocation();
  const uint32_t width = allocation.get_width();
  const uint32_t height = allocation.get_height();
  //const double xc = width/2.0;
  //const double yc = height/2.0;

  Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();

  cr->rectangle(event->area.x, event->area.y,
    event->area.width, event->area.height);
  cr->clip();

  cr->rectangle(0, 0, width, height);
  cr->set_source_rgb(1.0,0.6,0.6);
  cr->fill();
  cr->rectangle(100, 100, 200, 200);
  cr->set_source_rgb(0.0,0.0,0.0);
  cr->fill();

  return true;
}
