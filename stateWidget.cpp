#include "stateWidget.h"
#include <iostream>
#include <map>
#include <complex>

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

string convBase(unsigned long v, long base)
{
	string digits = "0123456789abcdef";
	string result;
	if((base < 2) || (base > 16)) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[v % base] + result;
			v /= base;
		}
		while(v);
	}
	return result;
}

string draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height, float mousex, float mousey){
	map<index_t, complex<float_t> >::const_iterator it;
  float xborder = width/12;
  float yborder = height/12;
	float rValue,iValue;//expectation value, real value, imaginary value
	float eValue, scale = 0;
  for (unsigned int i = 0; i < state->dim; i++) {//find max value for scaling
		rValue = state->data.find(i)->second.real();
		iValue = state->data.find(i)->second.imag();
		eValue = rValue*rValue+iValue*iValue;
    if (eValue > scale){
			scale = eValue;
		}
  }
	//scaled values
	float barWidth = (width-2*xborder)/((float)state->dim);
	float sHeight = height -2*yborder;

  for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = iValue*iValue + rValue*rValue;
			cr->set_source_rgb (1,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->fill();
	for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = iValue*iValue + rValue*rValue;
			cr->set_source_rgb (0,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->stroke();
	// XXX: hack
	int i = floor((float)(mousex-xborder)/barWidth);
  if (i >= 0 && (unsigned int)i < state->dim) {
		stringstream oss;
		string mystr;
    rValue = state->data.find(i)->second.real ();
		iValue = state->data.find(i)->second.imag ();
		eValue = iValue*iValue + rValue*rValue;

		oss << "State " << convBase ((unsigned long)i, 2) << " (" << i << ") amplitude " << rValue << " + " << iValue << "i (" << 100*eValue << "%)";
		return oss.str();
	}
	return "";
}
