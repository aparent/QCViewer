#include "circuitwidget.h"
#include "GateIcon.h"
#include <cairomm/context.h>
#include <circuit.h>
#include <circuitParser.h>
#include <simulate.h>
#include <iostream>
#include "draw.h"
#include <gtkmm.h>
#include <gate.h>
#include "draw.h"
#include "window.h" // slows down compiles, would be nice to not need this (see: clicking, effects toolpalette)

using namespace std;

CircuitWidget::CircuitWidget() :  circuit (NULL), cx(0), cy(0) {
  pan_mode = false;
  panning = drawarch = drawparallel = false;
  mode = NORMAL;
  NextGateToSimulate = 0;
  scale = 1.0;
  state = NULL;
  add_events (Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |Gdk::SCROLL_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &CircuitWidget::on_button_press_event));
  signal_button_release_event().connect(sigc::mem_fun(*this, &CircuitWidget::on_button_release_event) );
  signal_scroll_event().connect( sigc::mem_fun( *this, &CircuitWidget::onScrollEvent ) );
  signal_motion_notify_event().connect (sigc::mem_fun(*this, &CircuitWidget::onMotionEvent));
	cx = cy = 0;
  wirestart = wireend = 0;
  selecting = false;
}

void CircuitWidget::set_window (Gtk::Window *w) { win = w; }
void CircuitWidget::set_offset (int y) { yoffset = y; }

CircuitWidget::~CircuitWidget () {}
unsigned int CircuitWidget::getFirstWire (double my) {
  unsigned int ans=0;
  double mindist=0;
  for (unsigned int i = 0; i < circuit->numLines (); i++) {
    double y = wireToY (i);
    if (i == 0 || mindist > abs(my-y)) {
      mindist = abs(my-y);
      ans = i;
    }
  }
  return ans;

}

void CircuitWidget::clear_selection () {
  selections.clear ();
  force_redraw ();
}

void CircuitWidget::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time) {
  if (!circuit) { context->drag_finish(false, false, time); return; }
  selections.clear();
  ((QCViewer*)win)->set_selection (selections);
  Gtk::Widget* widget = drag_get_source_widget(context);
  Gtk::Button* button = dynamic_cast<Gtk::Button*>(widget);
  if (button == NULL) { context->drag_finish(false, false, time); return; }

  Gate *newgate = NULL;
  unsigned int target = 0;
  switch (((GateIcon*)button->get_image ())->type) {
    case GateIcon::NOT:
       newgate = new UGate ("X");
       newgate->drawType = Gate::NOT;
       break;
    case GateIcon::H: newgate = new UGate ("H"); break;
    case GateIcon::X: newgate = new UGate ("X"); break;
    case GateIcon::Y: newgate = new UGate ("Y"); break;
    case GateIcon::Z: newgate = new UGate ("Z"); break;
    case GateIcon::R: newgate = new RGate (1.0, RGate::Z); break;
    case GateIcon::SWAP:
      newgate = new UGate ("F");
      newgate->drawType = Gate::FRED;
      newgate->targets.push_back (target++);
      break;
    default: cout << "unhandled gate drag and drop" << endl; break;
  }
  if (newgate->targets.size () > circuit->numLines ()) {
    delete newgate;
    context->drag_finish (false, false, time);
    return;
  }
  newgate->targets.push_back(target++);
  Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  // translate mouse click coords into circuit diagram coords
  double xx = (x - width/2.0 + ext.width/2.0)/scale + cx;// - cx*scale;
  double yy = (y - height/2.0 + ext.height/2.0)/scale + cy;// - cy*scale;

  int column_id = pickRect (columns, xx, yy);
  unsigned int wire = getFirstWire (yy);
  if (wire + newgate->targets.size () - 1 >= circuit->numLines ()) wire = circuit->numLines () - newgate->targets.size ();
  for (unsigned int i = 0; i < newgate->targets.size(); i++) newgate->targets[i] += wire;
  if (columns.size () == 0) {
    insert_gate_at_front (newgate);
  } else if (column_id == -1) {
    if (yy < columns[0].y0 || yy - columns[0].y0 > columns[0].height) {
      // bad drag and drop
    } else if (xx < columns[0].x0) {
      insert_gate_at_front (newgate);
    } else {
      unsigned int i;
      for (i = 1; i < columns.size (); i++) {
        if (xx < columns[i].x0) {
          insert_gate_in_new_column (newgate, layout[i-1].lastGateID);
          break;
        }
      }
      if (i == columns.size ()) { // goes after all columns
        insert_gate_in_new_column (newgate, layout[i-1].lastGateID);
      }
    }
  } else {
    unsigned int start = (column_id == 0) ? 0 : layout[column_id - 1].lastGateID + 1;
    unsigned int end   = layout[column_id].lastGateID;
    bool ok = true;
    unsigned int mymaxwire, myminwire;
    mymaxwire = myminwire = newgate->targets[0];
    // TODO: holy crap this is too complicated! use minmaxWire!!!!!!!!!
    for (unsigned int j = 0; j < newgate->targets.size (); j++) {
      mymaxwire = max (mymaxwire, newgate->targets[j]);
      myminwire = min (myminwire, newgate->targets[j]);
    }
    for (unsigned int i = start; i <= end && ok; i++) {
      Gate* g = circuit->getGate (i);
      unsigned int maxwire, minwire;
      maxwire = minwire = g->targets[0];
      for (unsigned int j = 0; j < g->targets.size (); j++) {
        minwire = min (minwire, g->targets[j]);
        maxwire = max (maxwire, g->targets[j]);
      }
      for (unsigned int j = 0; j < g->controls.size (); j++) {
        minwire = min (minwire, g->controls[j].wire);
        maxwire = max (maxwire, g->controls[j].wire);
      }
      if (!(mymaxwire < minwire || myminwire > maxwire)) ok = false;
    }
    if (ok) {
      insert_gate_in_column (newgate, column_id);
    } else {
      insert_gate_in_new_column (newgate, end);
    }
  }

//  findInsertionPoint (layout, xx, yy, &column, &

  context->drag_finish(true, false, time);
}



bool CircuitWidget::on_button_press_event (GdkEventButton* event) {
  if (!circuit) return true;
  if (event->button == 1 && pan_mode) {
    panning = true;
    oldmousex = event->x;
    oldmousey = event->y;
  } else if (event->button == 1 && !pan_mode) {
    selecting = true;
    select_rect.x0 = event->x;
    select_rect.y0 = event->y;
    select_rect.width = 0;
    select_rect.height = 0;
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
  } else if (selecting) {
    select_rect.width = -select_rect.x0+event->x;
    select_rect.height = -select_rect.y0+event->y;
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
  Gate* g;
  if(event->button == 1 && panning) {
    panning = false;
  } else if (event->button == 1) {
    selecting = false;
    int column_id = -1.0; // before column 0
    double mindist = -1.0;
    int wireid;
    int gateid;
    double dist;
    unsigned int i;
    vector<Control>::iterator it;
    vector<unsigned int>::iterator it2;
    switch (mode) {
      case NORMAL:
        //gateid = pickRect (rects, x, y);
      {  gateRect r;
        r.x0 = (select_rect.x0-width/2.0+ext.width/2.0)/scale + cx;
        r.y0 = (select_rect.y0-height/2.0+ext.height/2.0)/scale + cx;
        r.width  = select_rect.width/scale;
        r.height = select_rect.height/scale;

        // make rect go in down/right direction
        r.x0 = min (r.x0, r.x0+r.width);
        r.y0 = min (r.y0, r.y0+r.height);
        r.width = abs(r.width);
        r.height = abs(r.height);

        cout << "w: " << r.width << "\th: " << r.height << "\n";
        selections.clear ();
        selections = pickRects (rects, r);
        //selections.clear ();
        //if (gateid != -1) selections.push_back (gateid);
        ((QCViewer*)win)->set_selection (selections);
        force_redraw ();
      }  break;
      case EDIT_CONTROLS:
        if (selections.size () != 1) {
          cout << "very bad thing happened: " << __LINE__ << __FILE__ << endl;
          selections.clear ();
          ((QCViewer*)win)->set_selection (selections);
        }
        // find out which wire was clicked
        // get the control associated with this gate, wire
        // if null, add it as a positive control
        // if ctrl is positive, add as negative
        // if ctrl is negative. remove.
        if (rects[selections[0]].x0 > x || rects[selections[0]].x0+rects[selections[0]].width < x) return true;
        wireid = pickWire (y);
        if ((unsigned int)wireid >= circuit->numLines()) wireid = -1;
        if (wireid == -1) return true;
        g = circuit->getGate (selections[0]);
        for (it = g->controls.begin (); it != g->controls.end (); ++it) {
          if (it->wire == (unsigned int)wireid) {
            it->polarity = !it->polarity;
            if (!it->polarity) { // instead of cycling pos/neg, delete if it /was/ neg.
              g->controls.erase (it);
              force_redraw ();
              return true;
            }
            force_redraw ();
            return true;
          }
        }
        { // XXX: sick of using switch due to the declaration stuff. switch it to if-else later.
          vector<unsigned int>::iterator it = find (g->targets.begin (), g->targets.end (), wireid);
          if (it != g->targets.end ()) return true; // if a target, can't be a control.
          g->controls.push_back (Control(wireid, false));
          // now, calculate whether adding this control will make this gate overlap with another.
          unsigned int col_id;
          for (col_id = 0; col_id < layout.size () && selections[0] > layout[col_id].lastGateID; col_id++);
          unsigned int minW, maxW;
          minmaxWire (&g->controls, &g->targets, &minW, &maxW);
          unsigned int firstGateID = col_id == 0 ? 0 : layout[col_id - 1].lastGateID + 1;
          for (unsigned int i = firstGateID; i <= layout[col_id].lastGateID; i++) {
            Gate* gg = circuit->getGate (i);
            unsigned int minW2, maxW2;
            minmaxWire (&gg->controls, &gg->targets, &minW2, &maxW2);
            if (i != selections[0] && !(minW2 > maxW || maxW2 < minW)) {
              circuit->swapGate (firstGateID, selections[0]); // pop it out to the left
              selections[0] = firstGateID;
              ((QCViewer*)win)->set_selection (selections);
              layout.insert (layout.begin()+col_id, LayoutColumn (firstGateID,0));
	      force_redraw ();
	      return true;
            }
          }
        }
        force_redraw ();
        return true;
        break;
      case EDIT_BREAKPOINTS:
        // note: the -1 on the range is so a breakpoint can't go after the last column. also can't go before first.
        for (i = 0; i < columns.size () - 1 && x >= columns[i].x0+columns[i].width; i++) {
          dist = abs(x - (columns[i].x0 + columns[i].width));
          if (dist < mindist || mindist == -1.0) { mindist = dist; column_id = i; }
        }
        if (column_id == -1) return true;
        it2 = find (breakpoints.begin (), breakpoints.end (), (unsigned int) column_id);
        if (it2 == breakpoints.end ()) {
          breakpoints.push_back ((unsigned int)column_id);
        } else {
          breakpoints.erase (it2);
        }
        force_redraw ();
        break;
    }
  }
  return true;
}
/* used to be able to select multiple gates. deemed silly. XXX: not so!
void CircuitWidget::toggle_selection (int id) {
  set <int>::iterator it;
  it = selections.find(id);
  if (it != selections.end()) selections.erase (it);
  else selections.insert (id);
  force_redraw ();
}*/

bool CircuitWidget::onScrollEvent (GdkEventScroll *event) {
  double s;
  if (event->direction == 1) s = get_scale()/1.15;
  else s = get_scale()*1.15;
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
    cr->rectangle(event->area.x, event->area.y,
                  event->area.width, event->area.height);
    cr->clip();
    cr->rectangle (0, 0, width, height);
    cr->set_source_rgb (1,1,1);
    cr->fill ();
    cr->translate (xc-ext.width/2.0-cx*scale, yc-ext.height/2.0-cy*scale);
    if (circuit != NULL) {
      rects = draw_circuit (circuit, cr->cobj(), layout, drawarch, drawparallel,  ext, wirestart, wireend, scale, selections);
      generate_layout_rects ();
      for (unsigned int i = 0; i < NextGateToSimulate; i++) {
        drawRect (cr->cobj(), rects[i], Colour (0.1,0.7,0.2,0.7), Colour (0.1, 0.7,0.2,0.3));
      }
      if (false) {
        for (unsigned int i = 0; i < layout.size (); i++) {
          drawRect (cr->cobj(), columns[i], Colour (0.3, 0.3,0.3,0.7), Colour (0.3,0.3,0.3,0.3));
        }
      }
      for (unsigned int i = 0; i < breakpoints.size (); i++) {
        unsigned int j = breakpoints[i];
        double x = (columns[j].x0+columns[j].width+columns[j+1].x0)/2.0;
        double y = (0 - height/2.0 + ext.height/2.0)/scale + cy;
        cr->set_source_rgba (0.8,0,0,0.8);
        cr->move_to (x, y);
        cr->line_to (x, y+height/scale);
        cr->stroke ();
      }
      if (selecting) {
        gateRect r;
        r.x0 = (select_rect.x0-width/2.0+ext.width/2.0)/scale + cx;
        r.y0 = (select_rect.y0-height/2.0+ext.height/2.0)/scale + cx;
        r.width  = select_rect.width/scale;
        r.height = select_rect.height/scale;
        drawRect (cr->cobj(), r, Colour (0.7,0.7,0.7,0.7), Colour (0.7,0.7,0.7,0.3));
      }
    }
  }
  return true;
}

void CircuitWidget::newcircuit (unsigned int numqubits) {
  if (circuit != NULL) delete circuit;
  circuit = new Circuit ();
  layout.clear ();
  breakpoints.clear ();
  for (unsigned int i = 0; i < numqubits; i++) {
    stringstream ss;
    ss << i + 1;
    circuit->addLine (ss.str ());
    Line* l = circuit->getLine (i);
    l->outLabel = ss.str ();
    l->constant = false;
    l->garbage = false;
  }
}

void CircuitWidget::load (string file) {
  if (circuit != NULL) delete circuit;
  circuit = parseCircuit(file);
  layout.clear ();
  breakpoints.clear ();
  vector<int> parallels = circuit->getGreedyParallel ();

  for (unsigned int i = 0; i < parallels.size(); i++) {
    layout.push_back (LayoutColumn(parallels[i], 0.0));
  }
  layout[parallels.size () - 1].pad = 0.0;
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

void CircuitWidget::save_circuit (string filename) {
	saveCircuit(circuit,filename);
}
void CircuitWidget::savepng (string filename) {
  if (!circuit) return;
  double wirestart, wireend;
  cairo_rectangle_t ext = get_circuit_size (circuit, layout, &wirestart, &wireend, 1.0);

  cairo_surface_t* surface = make_png_surface (ext);
  cairo_t* cr = cairo_create (surface);
  cairo_set_source_surface (cr, surface, 0, 0);
  draw_circuit (circuit, cr, layout, drawarch, drawparallel,  ext, wirestart, wireend, 1.0, vector<uint32_t>());
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
  draw_circuit (circuit, cr, layout, drawarch, drawparallel, ext, wirestart, wireend, 1.0, vector<uint32_t>());
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
}

void CircuitWidget::set_scale (double x) {
  scale = x;
  ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
  force_redraw ();
}

// XXX: urg, may be duplicated elsewhere. check.
unsigned int findcolumn (vector<LayoutColumn>& layout, unsigned int gate) {
  unsigned int i;
  for (i = 0; i < layout.size () && gate > layout[i].lastGateID; i++);
  return i - 1;
}

bool CircuitWidget::run (bool breaks) {
  if (!circuit || !state) return false;
  if (NextGateToSimulate == circuit->numGates ()) NextGateToSimulate = 0;
  // always step over first breakpoint if it is around
  while (NextGateToSimulate < circuit->numGates ()) {
    *state = ApplyGate (state, circuit->getGate (NextGateToSimulate));
    if (!state) { force_redraw (); return false; }
    NextGateToSimulate++;
    if (!breaks) continue;
    // check if we have reached a breakpoint
    // find column that gate is in. if 0, ignore. else, check if gateid is one more than previous layout column last id. if so, return true.
    unsigned int column_id = findcolumn (layout, NextGateToSimulate);
    vector<unsigned int>::iterator it = find (breakpoints.begin (), breakpoints.end (), column_id );
    if (it == breakpoints.end ()) continue;
    if (layout[column_id].lastGateID + 1 == NextGateToSimulate) { force_redraw (); return true; }
  }
  force_redraw ();
  return false;
}

bool CircuitWidget::step () {
  if (!circuit || !state) return false;
  if (NextGateToSimulate < circuit->numGates ()) {
    *state = ApplyGate(state,circuit->getGate(NextGateToSimulate));
    if (!state) return false;
    NextGateToSimulate++;
//    state->print();
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

void CircuitWidget::insert_gate_in_column (Gate *g, unsigned int column_id) {
  for (unsigned int j = column_id; j < layout.size (); j++) layout[j].lastGateID += 1;
  circuit->addGate(g, layout[column_id].lastGateID - 1);
  ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
  force_redraw ();
  selections.clear ();
  selections.push_back(layout[column_id].lastGateID - 1);
  ((QCViewer*)win)->set_selection (selections);
}

void CircuitWidget::insert_gate_at_front (Gate *g) {
  for (unsigned int j = 0; j < layout.size (); j++) layout[j].lastGateID += 1;
  circuit->addGate(g, 0);
  layout.insert(layout.begin(), LayoutColumn (0,0));
  ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
  force_redraw ();
  selections.clear ();
  selections.push_back(0);
  ((QCViewer*)win)->set_selection (selections);
}

// XXX: this may actually be more complicated than necessary now.
void CircuitWidget::insert_gate_in_new_column (Gate *g, unsigned int x) {
  if (!circuit) return;
  unsigned int i;
  for (i = 0; i < layout.size() && layout[i].lastGateID < x; i++);
  unsigned int pos = layout[i].lastGateID + 1;
  for (unsigned int j = i + 1; j < layout.size (); j++) layout[j].lastGateID += 1;
  circuit->addGate (g, pos);
  layout.insert (layout.begin() + i + 1, LayoutColumn (pos, 0));
  ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
  force_redraw ();
  selections.clear ();
  selections.push_back (pos);
  ((QCViewer*)win)->set_selection (selections);
}

void CircuitWidget::delete_gate (unsigned int id) {
  if (!circuit) return;
  unsigned int i = 0;
  selections.clear ();
  NextGateToSimulate = 0;
  for (i = 0; i < layout.size(); i++) {
    if (layout[i].lastGateID > id) break;
    if (layout[i].lastGateID < id) continue;
    // layout[i].lastGateID == id
    if (i == 0 && id != 0) break;
    if (i == 0 || layout[i - 1].lastGateID == id - 1) {
      vector<unsigned int>::iterator it = find (breakpoints.begin (), breakpoints.end (), i);
      if (it != breakpoints.end ()) breakpoints.erase (it);
      layout.erase (layout.begin() + i);
      break;
    } else break;
  }
  for (; i < layout.size (); i++) layout[i].lastGateID -= 1;
  circuit->removeGate (id);
  ext = get_circuit_size (circuit, layout, &wirestart, &wireend, scale);
  force_redraw ();
}

void CircuitWidget::generate_layout_rects () {
  columns.clear ();
  if (!circuit || circuit->numGates () == 0) return;
  unsigned int start_gate = 0;
  for (unsigned int column = 0; column < layout.size() && start_gate < circuit->numGates (); column++) {
    gateRect bounds = rects[start_gate];
    for (unsigned int gate = start_gate + 1; gate <= layout[column].lastGateID; gate++) {
      bounds = combine_gateRect(bounds, rects[gate]);
    }
    bounds.y0 = ext.y;
    bounds.height = max (bounds.height, ext.height);
    columns.push_back(bounds);
    start_gate = layout[column].lastGateID + 1;
  }
}

void CircuitWidget::set_mode (Mode m) { mode = m; }

Gate *CircuitWidget::getSelectedGate () {
  if (!circuit || selections.size () != 1) {
    if (selections.size () > 1) cout << "bad: getSelectedGate when multiple gates selected.\n";
    return NULL;
  }
  return circuit->getGate(selections[0]);
}

void  CircuitWidget::arch_set_LNN(){
	circuit->arch_set_LNN();
}

void CircuitWidget::set_panning (bool p) {
  pan_mode = p;
}
