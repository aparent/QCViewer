#include "circuitwidget.h"
#include "GateIcon.h"
#include <cairomm/context.h>
#include <circuit.h>
#include <circuitParser.h>
#include <simulate.h>
#include <iostream>
#include "draw.h"
#include <gtkmm.h>
#include <sstream> // XXX: itoa, remove later

using namespace std;

CircuitWidget::CircuitWidget() : panning (false), drawarch (false), drawparallel (false), circuit (NULL), selection (-1)  {
  NextGateToSimulate = 0;
	scale = 1.0;
	state = NULL;
  add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &CircuitWidget::on_button_press_event));
  signal_button_release_event().connect(sigc::mem_fun(*this, &CircuitWidget::on_button_release_event) );
  signal_scroll_event().connect( sigc::mem_fun( *this, &CircuitWidget::onScrollEvent ) );
  signal_motion_notify_event().connect (sigc::mem_fun(*this, &CircuitWidget::onMotionEvent));
}

void CircuitWidget::set_window (Gtk::Window *w) { win = w; }
void CircuitWidget::set_offset (int y) { yoffset = y; }

CircuitWidget::~CircuitWidget () {}

void CircuitWidget::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time) {
	if (!circuit) { context->drag_finish(false, false, time); return; }
	Gtk::Widget* widget = drag_get_source_widget(context);
  Gtk::ToolPalette* drag_palette = dynamic_cast<Gtk::ToolPalette*>(widget);
	while(widget && !drag_palette) { // TODO: uh i should figure out what this actually does (magic)
	  widget = widget->get_parent();
		drag_palette = dynamic_cast<Gtk::ToolPalette*>(widget);
	}
  Gtk::ToolItem* drag_item = NULL;
	if (drag_palette)
    drag_item = drag_palette->get_drag_item(selection_data);
  Gtk::ToolButton* button = dynamic_cast<Gtk::ToolButton*>(drag_item);
  if(!button)
	  return;

  Gate *newgate;
	unsigned int target = 0;
	switch (((GateIcon*)button->get_icon_widget ())->type) {
		case GateIcon::NOT:
		   newgate = new UGate ("X");
			 newgate->drawType = NOT;
		   break;
		case GateIcon::H: newgate = new UGate ("H"); break;
		case GateIcon::X: newgate = new UGate ("X"); break;
		case GateIcon::Y: newgate = new UGate ("Y"); break;
		case GateIcon::Z: newgate = new UGate ("Z"); break;
		case GateIcon::R: newgate = new RGate (1.0); break;
		case GateIcon::SWAP:
		  newgate = new UGate ("F");
			newgate->drawType = FRED;
			newgate->targets.push_back (target++);
	    break;
		default: cout << "unhandled gate drag and drop"; break;
	}
	newgate->targets.push_back(target);
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  // translate mouse click coords into circuit diagram coords
  double xx = (x - width/2.0 + ext.width/2.0)/scale + cx;// - cx*scale;
  double yy = (y - height/2.0 + ext.height/2.0)/scale + cy;// - cy*scale;
	int i = pickRect (rects, xx, yy);
	cout << "i  is " << i << endl;
	insert_gate (newgate, 0);
  context->drag_finish(true, false, time);
}

bool CircuitWidget::on_button_press_event (GdkEventButton* event) {
	if (!circuit) return true;
  if (event->button == 3) {
    panning = true;
    oldmousex = event->x;
    oldmousey = event->y;
  }
  return true;
}

bool CircuitWidget::onMotionEvent (GdkEventMotion* event) {
	if (!circuit) return true;
  if (panning) {
    cx -= (event->x - oldmousex)/scale;
    cy -= (event->y - oldmousey)/scale;
    oldmousex = event->x;
    oldmousey = event->y;
    force_redraw ();
  }
  return true;
}

bool CircuitWidget::on_button_release_event(GdkEventButton* event) {
	if (!circuit) return true;
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  // translate mouse click coords into circuit diagram coords
  double x = (event->x - width/2.0 + ext.width/2.0)/scale + cx;// - cx*scale;
  double y = (event->y - height/2.0 + ext.height/2.0)/scale + cy;// - cy*scale;
  if(event->button == 3 && panning) {
    panning = false;
  } else if (event->button == 1) {
    int i = pickRect (rects, x, y);
    cout << "click! at (" << event->x << ", " << event->y << ") ";
    cout << "which translates to (" << x << ", "<< y << ")" << endl << flush;
    if (i == -1) cout << "no gate clicked..." << endl << flush;
    else { cout << "clicked gate " << i << endl << flush; }

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
    cr->translate (xc-ext.width/2.0-cx*scale, yc-ext.height/2.0-cy*scale);
    //cr->clip();
    if (circuit != NULL) {
      rects = draw_circuit (circuit, cr->cobj(), layout, drawarch, drawparallel,  ext, wirestart, wireend, scale, selection);
      for (unsigned int i = 0; i < NextGateToSimulate; i++) {
        drawRect (cr->cobj(), rects[i], Colour (0.1,0.7,0.2,0.7), Colour (0.1, 0.7,0.2,0.3));
      }
    }
		/*cr->set_matrix (Cairo::identity_matrix());
		cr->rectangle (xc-5,yc-5,10,10);
		cr->set_source_rgb (0,0,1);
    cr->fill ();*/
  }
  return true;
}

void CircuitWidget::load (string file) {
  if (circuit != NULL) delete circuit;
  circuit = parseCircuit(file);
  layout.clear ();
  vector<int> parallels = circuit->getGreedyParallel ();
  for (unsigned int i = 0; i < parallels.size(); i++)
    layout.push_back (LayoutColumn(parallels[i], 20.0));
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
  cairo_rectangle_t ext = get_circuit_size (circuit, layout, &wirestart, &wireend, 1.0);

  cairo_surface_t* surface = make_png_surface (ext);
  cairo_t* cr = cairo_create (surface);
  cairo_set_source_surface (cr, surface, 0, 0);
  draw_circuit (circuit, cr, layout, drawarch, drawparallel,  ext, wirestart, wireend, 1.0, -1);
  write_to_png (surface, filename);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
}

void CircuitWidget::savesvg (string filename) {
  if (!circuit) return;
  double wirestart, wireend;
  cairo_rectangle_t ext = get_circuit_size (circuit, layout, &wirestart, &wireend, 1.0);
  cairo_surface_t* surface = make_svg_surface (filename, ext);
  cairo_t* cr = cairo_create (surface);
  cairo_set_source_surface (cr, surface, 0, 0);
  draw_circuit (circuit, cr, layout, drawarch, drawparallel, ext, wirestart, wireend, 1.0, -1);
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
}

void CircuitWidget::set_scale (double x) {
  scale = x;
  ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
  force_redraw ();
}

bool CircuitWidget::step () {
  if (!circuit || !state) return false;
  if (NextGateToSimulate < circuit->numGates ()) {
    *state = ApplyGate(state,circuit->getGate(NextGateToSimulate));
		if (!state) return false;
    NextGateToSimulate++;
//		state->print();
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

void CircuitWidget::insert_gate (Gate *g, unsigned int x) {
  cout << "\n\n\nInserting gate after pos " << x << "...\n";
  cout << "layout";
  for (unsigned int k = 0; k < layout.size (); k++) cout << " " << layout[k].lastGateID;
  cout << ".\n";
  if (!circuit) return;
  unsigned int i;
  for (i = 0; i < layout.size() && layout[i].lastGateID < x; i++);
  unsigned int pos = layout[i].lastGateID + 1;
  cout << "Column " << i << " has lastGateID = " << layout[i].lastGateID << ".\n" << flush;
  cout << "Will insert gate as gate # "<< pos << ".\n";
  for (unsigned int j = i + 1; j < layout.size (); j++) { cout << "gate " << layout[j].lastGateID << "++\n"; layout[j].lastGateID += 1; }
  circuit->addGate (g, pos);
//  cout << "gate[pos].name = " << circuit->getGate(pos)->name << "\n";
  layout.insert (layout.begin() + i + 1, LayoutColumn (pos, 0)); 
  cout << "layout";
  for (unsigned int k = 0; k < layout.size (); k++) cout << " " << layout[k].lastGateID;
  cout << ".\n";
  ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
  force_redraw ();
}

void CircuitWidget::delete_gate () {
	if (selection == -1 || !circuit) return;
  unsigned int i = 0;
	for (i = 0; i < layout.size(); i++) {
    if (layout[i].lastGateID > (unsigned int)selection) break;
    if (layout[i].lastGateID < (unsigned int)selection) continue;
    // layout[i].lastGateID == id
    if (i == 0 || layout[i - 1].lastGateID == (unsigned int)selection - 1) {
      layout.erase (layout.begin() + i);
    } else break;
  }
  for (; i < layout.size (); i++) layout[i].lastGateID -= 1;
	circuit->removeGate (selection);
	ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
	force_redraw ();
}

void CircuitWidget::set_insert (bool x) {
  insert = x;
	selection = 01;
}
