#include <iostream>
#include <map>
#include <complex>
#include <utility.h>
#include "stateWidget.h"
#include <vector>

using namespace std;

StateViewWidget::StateViewWidget (Gtk::Statusbar* ns, Gtk::HBox* h, vector<StateViewWidget*>* svw, Gtk::VPaned* pane) :
    sw(ns), btn_expected ("Expected"),  btn_real ("Real"), btn_imag ("Imag"), btn_close ("Close") ,btn_trace ("Trace"){
  visbox = h;
  svwList = svw;
  vispane = pane;
  Gtk::RadioButton::Group group = btn_expected.get_group ();
  btn_real.set_group (group);
  btn_imag.set_group (group);
  buttonbox.pack_start (btn_expected, Gtk::PACK_SHRINK);
  buttonbox.pack_start (btn_real, Gtk::PACK_SHRINK);
  buttonbox.pack_start (btn_imag, Gtk::PACK_SHRINK);
  buttonbox.pack_start (btn_trace, Gtk::PACK_SHRINK);
  buttonbox.pack_start (btn_close, Gtk::PACK_SHRINK);
  pack_start (sw);
  pack_start (buttonbox, Gtk::PACK_SHRINK);
  btn_expected.signal_clicked().connect(sigc::mem_fun(*this, &StateViewWidget::set_style));
  btn_real.signal_clicked().connect(sigc::mem_fun(*this, &StateViewWidget::set_style));
  btn_imag.signal_clicked().connect(sigc::mem_fun(*this, &StateViewWidget::set_style));
  btn_trace.signal_clicked().connect(sigc::mem_fun(*this, &StateViewWidget::set_style));
  btn_close.signal_clicked().connect(sigc::mem_fun(*this, &StateViewWidget::close));
  btn_expected.set_active ();
  set_style ();
  show_all_children ();
}

void StateViewWidget::close () {
  visbox->remove (*this);
  vector<StateViewWidget*>::iterator it = find (svwList->begin (), svwList->end (), this);
  if (it == svwList->end ()) cout << "VERY BAD WARNING\n";
  svwList->erase (it);
  if (svwList->size () == 0) {
    vispane->set_position (3000); // TODO: do this better
  }
}

void StateViewWidget::set_style () {
	StateWidget::DrawMode d;
	if (btn_real.get_active ())     		d = StateWidget::STATEDRAW_REAL;
	else if (btn_imag.get_active ())    d = StateWidget::STATEDRAW_IMAG;
	else if (btn_trace.get_active ())   d = StateWidget::STATEDRAW_EXPECTED_TRACED;
	else 																d = StateWidget::STATEDRAW_EXPECTED;
	if (d != sw.drawmode) {
		sw.drawmode = d;
	  sw.reset ();
	}
}

void StateViewWidget::reset () {
  sw.reset ();
}

void StateViewWidget::set_state (State* state) {
  sw.set_state (state);
}

StateWidget::StateWidget(Gtk::Statusbar* ns) {
  state = NULL;
  status = ns;
  add_events (Gdk::POINTER_MOTION_MASK);
  signal_motion_notify_event().connect (sigc::mem_fun(*this, &StateWidget::onMotionEvent));
  mousex = mousey = 0;
  drawmode = STATEDRAW_EXPECTED;
  num_draw = 0;
	trace = 63;
  set_size_request (100,100);
}

// TODO: this is ugly
#define EPS 0.000001

bool StateWidget::on_expose_event (GdkEventExpose* event) {
  (void)event; // placate compiler...
  Glib::RefPtr<Gdk::Window> window = get_window ();
  if (window) {
    Gtk::Allocation allocation = get_allocation ();
    width = allocation.get_width ();
    height = allocation.get_height ();
    xborder = 0.05*width;
    yborder = 0.02*height;
    tickwidth = 0.5*xborder;
    barWidth = (width - 2.0*xborder)/((double)num_draw);
    t_barWidth = (width - 2.0*xborder)/((double)num_draw_traced);
    barHeight = height - 2.0*yborder;

    Cairo::RefPtr <Cairo::Context> cr = window->create_cairo_context ();
    cr->rectangle (0, 0, width, height);
    cr->set_source_rgb (1, 1, 1);
    cr->fill ();
    if (drawmode == STATEDRAW_EXPECTED || drawmode == STATEDRAW_EXPECTED_TRACED) {
      if (state != NULL) {
				if (drawmode == STATEDRAW_EXPECTED){
        	for (unsigned int i = 0; i < num_draw; i++) {
          	double eValue = bucket[i].real()*bucket[i].real() + bucket[i].imag()*bucket[i].imag();
          	if (eValue > EPS) {
          		cr->rectangle (xborder + ((double)i)*barWidth, height - yborder, barWidth, -eValue*barHeight);
          	  cr->set_source_rgb (0, 0, 0);
          	  cr->stroke_preserve ();
          	  cr->set_source_rgb (0, 1, 0);
          	  cr->fill ();
          	}
        	}
				} else {
					for (unsigned int i = 0; i < num_draw_traced; i++) {
          	if (traced_bucket[i] > EPS) {
          		cr->rectangle (xborder + ((double)i)*t_barWidth, height - yborder, t_barWidth, -traced_bucket[i]*barHeight);
          	  cr->set_source_rgb (0, 0, 0);
          	  cr->stroke_preserve ();
          	  cr->set_source_rgb (0, 1, 0);
          	  cr->fill ();
					}
      	}
			}
      cr->set_source_rgb (0.5, 0.5, 0.5);
      cr->move_to (xborder, height - yborder);
      cr->line_to (xborder, yborder);
      cr->stroke ();
      cr->move_to (xborder, height - yborder);
      cr->line_to (width - xborder, height - yborder);
      cr->stroke ();
      cr->move_to (xborder-tickwidth/2.0, yborder);
      cr->line_to (xborder+tickwidth/2.0, yborder);
      cr->stroke ();
			}
    } else if (drawmode == STATEDRAW_REAL || drawmode == STATEDRAW_IMAG) { // XXX: proper scaling?
      double avg = 0.0;
      if (state != NULL) {
        double maxX = 0.0;
        for (unsigned int i = 0; i < num_draw; i++) {
          double val = drawmode == STATEDRAW_REAL ? bucket[i].real() : bucket[i].imag ();
          avg += val;
          maxX = max (abs(val), maxX);
        }
        for (unsigned int i = 0; i < num_draw; i++) {
          float_t eValue = (drawmode == STATEDRAW_REAL ? bucket[i].real() : bucket[i].imag ())/maxX;
          if (abs(eValue) > EPS) {
            cr->rectangle (xborder + (double)i*barWidth, height/2.0, barWidth, -eValue*barHeight/2.0);
            cr->set_source_rgb (0, 0, 0);
             cr->stroke_preserve ();
             cr->set_source_rgb (drawmode == STATEDRAW_REAL ? 1 : 0, 0, drawmode == STATEDRAW_IMAG ? 1 : 0);
             cr->fill ();
          }
        }
        avg /= num_draw;
        cr->set_source_rgba (0.5, 0.5, 0.5, 0.5);
        double y = (height-avg/maxX*barHeight)/2.0;
        cr->move_to (xborder, y);
        cr->line_to (width - xborder, y);
        cr->stroke ();
      }
      cr->set_source_rgb (0.5, 0.5, 0.5);
      cr->move_to (xborder, height/2.0);
      cr->line_to (width - xborder, height/2.0);
      cr->stroke ();
      cr->move_to (xborder, yborder);
      cr->line_to (xborder, height-yborder);
      cr->stroke ();
      cr->move_to (xborder - tickwidth/2.0, yborder);
      cr->line_to (xborder + tickwidth/2.0, yborder);
      cr->stroke ();
      cr->move_to (xborder - tickwidth/2.0, height - yborder);
      cr->line_to (xborder + tickwidth/2.0, height - yborder);
      cr->stroke ();
    }
  }
  return true;
}

string base2enc (unsigned long v, unsigned int len) {
  string result = "";
  do {
    result = (char)('0' + (v & 0x1)) + result;
    v >>= 1;
  } while(v);
  if (result.length () < len) {
    len = len - result.length ();
    while (len--) result = '0' + result;
  }
  return result;
}

bool StateWidget::onMotionEvent (GdkEventMotion* event) {
  if (status == NULL) return true;
  mousex = event->x;
  mousey = event->y;
	unsigned int i=0;
	if (drawmode!=STATEDRAW_EXPECTED_TRACED) i = floor ((mousex - xborder)/barWidth);
	else i = floor ((mousex - xborder)/t_barWidth);
  if (i >= 0 && i < num_draw) {
    stringstream oss;
    string mystr;
    int dim = 0;
		if (drawmode!=STATEDRAW_EXPECTED_TRACED){
    	double eValue = bucket[i].real()*bucket[i].real() + bucket[i].imag()*bucket[i].imag();
    	int twotothedim = state->dim;
    	while (twotothedim >>= 1) dim++; // TODO: we should have size/dim encoded in the state vector.
			oss << "State " << base2enc ((unsigned long)bucketID[i], dim) << " (" << bucketID[i] << ") amplitude " << bucket[i].real() << " + " << bucket[i].imag () << "i (" << 100.0*eValue << "%)";
		} else{
    	unsigned int twotothedim = num_draw_traced;
    	while (twotothedim >>= 1) dim++; // TODO: we should have size/dim encoded in the state vector.
			oss << "State " << base2enc ((unsigned long)i, dim) << " (" << i << ")(" << 100.0*traced_bucket[i] << "%)";
		}
    status->pop ();
    status->push (oss.str());
  } else {
    status->pop ();
  }
  return true;
}

void StateWidget::reset () {
  if (state != NULL) {
    parse_state ();
		parse_state_trace ();
  }
  force_redraw ();
}

void StateWidget::set_state(State* n_state){
  state = n_state;
  reset ();
}

void StateWidget::set_trace(index_t n_trace){
  trace = n_trace;
  reset ();
}

void StateWidget::force_redraw () {
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win) {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                     get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
}


// XXX: this needs to be changed a lot to support more than 32 qubits
void StateWidget::parse_state () {
  if (state->dim >= numBuckets) { draw_compressed = true; }
  else { draw_compressed = false; num_draw = state->dim; }
  unsigned int skip = ceil ((double)state->dim/(double)numBuckets);
  unsigned int n = 0;
  for (unsigned int i = 0; i < numBuckets; i++) {
    bucket[i] = 0.0;
		unsigned int maxID = 0; // XXX: should be bit string
		double maxMag = -1;
		for (unsigned int j = 0; j < skip; j++, n++) {
      StateMap::iterator it = state->data.find (n); // XXX: why is std:: required here?
			if (it == state->data.end ()) continue;
      bucket[i] += it->second;
      double mag = it->second.real()*it->second.real() + it->second.imag()*it->second.imag();
      maxID = mag > maxMag ? n : maxID;
    }
    bucketID[i] = maxID;
    //bucket[i] /= skip;  Would average might want to just add probs?
  }
}

void StateWidget::parse_state_trace () {
	//Traced Part.
  index_t basis_size = bitcount(trace);
  index_t state_size = ipow(2,basis_size);
	num_draw_traced = state_size;
  if (state_size >= numBuckets) {
		cout << "Error Too large for trace" << endl;
		return;
	}
	for (unsigned int i = 0; i < numBuckets; i++){
		traced_bucket[i] = 0.0;
	}
	for (StateMap::iterator it = state->data.begin(); it != state->data.end(); it++){
    traced_bucket[ExtractBits(it->first,trace)] += it->second.real()*it->second.real() + it->second.imag()*it->second.imag();
	}
}
