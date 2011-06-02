#include "circuitwidget.h"
#include <cairomm/context.h>
#include <circuit.h>
#include <circuitParser.h>
#include <simulate.h>
#include <iostream>
#include "draw.h"
#include <gtkmm.h>

using namespace std;

CircuitWidget::CircuitWidget() : panning (false), drawarch (false), drawparallel (false), circuit (NULL), selection (-1)  {
  NextGateToSimulate = 0;
  add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &CircuitWidget::on_button_press_event));
  signal_button_release_event().connect(sigc::mem_fun(*this, &CircuitWidget::on_button_release_event) );
  signal_scroll_event().connect( sigc::mem_fun( *this, &CircuitWidget::onScrollEvent ) );
  signal_motion_notify_event().connect (sigc::mem_fun(*this, &CircuitWidget::onMotionEvent));
}

void CircuitWidget::set_window (Gtk::Window *w) { win = w; }
void CircuitWidget::set_offset (int y) { yoffset = y; }

CircuitWidget::~CircuitWidget () {}

bool CircuitWidget::on_button_press_event (GdkEventButton* event) {
  if (event->button == 1) {
    panning = true;
    oldmousex = event->x;
    oldmousey = event->y;
  }
  return true;
}

bool CircuitWidget::onMotionEvent (GdkEventMotion* event) {
  if (panning) {
    cx -= (event->x - oldmousex);
    cy -= (event->y - oldmousey); 
    oldmousex = event->x;
    oldmousey = event->y;
    force_redraw ();
  }
  return true;
}

bool CircuitWidget::on_button_release_event(GdkEventButton* event) {
  if(event->button == 1 && panning) {
    panning = false;
  } else if (event->button == 3) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
    // translate mouse click coords into circuit diagram coords
    double x = (event->x - width/2.0) + ext.width/2.0 + cx;
    double y = (event->y - height/2.0) + ext.height/2.0 + cy;
  
    cout << "click! at (" << event->x << ", " << event->y << ") ";
    cout << "which translates to (" << x << ", "<< y << ")" << endl << flush;
    int i = pickRect (rects, x, y);
    if (i == -1) cout << "no gate clicked..." << endl << flush;
    else cout << "clicked gate " << i << endl << flush;

    selection = i;
    force_redraw ();
  }
  return true;
}
/* used to be able to select multiple gates. deemed silly.
void CircuitWidget::toggle_selection (int id) {
  set <int>::iterator it;
  it = selections.find(id);
  if (it != selections.end()) selections.erase (it);
  else selections.insert (id);
  force_redraw ();
}*/

bool CircuitWidget::onScrollEvent (GdkEventScroll *event) {
  double s;
  if (event->direction == 1) s = get_scale()/1.25;
  else s = get_scale()*1.25;
  set_scale(s);
  force_redraw ();
  return true;
}

bool CircuitWidget::on_expose_event(GdkEventExpose* event) {
  (void)event; // placate compiler..
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  if(window) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    double xc = width/2.0;
    double yc = height/2.0;

    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
//    cr->rectangle(event->area.x, event->area.y,
//                  event->area.width, event->area.height);
    cr->rectangle (0, 0, width, height);
    cr->set_source_rgb (1,1,1);
    cr->fill ();
    cr->translate (xc-ext.width/2-cx, yc-ext.height/2-cy);
    //cr->clip();
    if (circuit != NULL) {
      rects = draw_circuit (circuit, cr->cobj(), drawarch, drawparallel,  ext, wirestart, wireend, scale, selection);
      for (unsigned int i = 0; i < NextGateToSimulate; i++) {
        drawRect (cr->cobj(), rects[i], Colour (0.1,0.7,0.2,0.7), Colour (0.1, 0.7,0.2,0.3));
      }
    }
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
}

void CircuitWidget::loadArch (string file) {
  if (circuit) { circuit->parseArch (file); force_redraw (); }
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

void CircuitWidget::savepng (string filename) {
  if (!circuit) return;
  double wirestart, wireend;
  cairo_rectangle_t ext = get_circuit_size (circuit, &wirestart, &wireend, 1.0);

  cairo_surface_t* surface = make_png_surface (ext);
  cairo_t* cr = cairo_create (surface);
  cairo_set_source_surface (cr, surface, 0, 0);
  draw_circuit (circuit, cr, drawarch, drawparallel,  ext, wirestart, wireend, 1.0, -1);
  write_to_png (surface, filename);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
}

void CircuitWidget::savesvg (string filename) {
  if (!circuit) return;
  double wirestart, wireend;
  cairo_rectangle_t ext = get_circuit_size (circuit, &wirestart, &wireend, 1.0);
  cairo_surface_t* surface = make_svg_surface (filename, ext);
  cairo_t* cr = cairo_create (surface);
  cairo_set_source_surface (cr, surface, 0, 0);
  draw_circuit (circuit, cr, drawarch, drawparallel, ext, wirestart, wireend, 1.0, -1);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
}

void CircuitWidget::set_scale (double x) { 
  scale = x; 
  ext = get_circuit_size (circuit, &wirestart, &wireend, scale);
  force_redraw ();
}

bool CircuitWidget::step () {
  if (!circuit) return false;
  if (NextGateToSimulate < circuit->numGates ()) {
		cout << state->data.size()<< endl;
    *state = ApplyGate(state,circuit->getGate(NextGateToSimulate));
    NextGateToSimulate++;
    force_redraw ();
    return true;
  } else {
    return false;
  }
}

void CircuitWidget::reset () {
  if (circuit && NextGateToSimulate != 0) {
    NextGateToSimulate = 0;
    force_redraw ();
  }
}

void CircuitWidget::set_state (State* n_state){
	state = n_state;
}

double CircuitWidget::get_scale () { return scale; }
int CircuitWidget::get_QCost () { return circuit->QCost(); }
int CircuitWidget::get_Depth () { return circuit->getParallel().size(); }
int CircuitWidget::get_NumGates () { return circuit->numGates(); }
