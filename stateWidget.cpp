#include "stateWidget.h"
#include <string>
using namespace std;

StateWidget::StateWidget() : state (NULL) {
  add_events (Gdk::POINTER_MOTION_MASK);
  signal_motion_notify_event().connect (sigc::mem_fun(*this, &StateWidget::onMotionEvent));
	mousex = mousey = 0;
}

bool StateWidget::on_expose_event(GdkEventExpose* event) {
  (void)event; // placate compiler..
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->rectangle (0, 0, width, height);
    cr->set_source_rgb (1,1,1);
    cr->fill();
    if (state != NULL) {
			string status = draw_state (cr, state,(float)width,(float)height, mousex, mousey);
			m_statusbar->pop ();
      m_statusbar->push(status);
		}
  }
  return true;
}

bool StateWidget::onMotionEvent (GdkEventMotion* event) {
  mousex = event->x;
	mousey = event->y;
	force_redraw ();
	return true;
}

void StateWidget::set_state(State* n_state){
	state = n_state;
	force_redraw();
}

void StateWidget::force_redraw () {
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                     get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}
