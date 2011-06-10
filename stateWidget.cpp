#include "stateWidget.h"
#include <iostream>
#include <map>
#include <complex>

using namespace std;

StateWidget::StateWidget() : state (NULL) {}

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
    if (state != NULL)draw_state (cr, state,(float)width,(float)height);
  }
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

void StateWidget::draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height){
	StateMap::const_iterator it;
	float xborder = width/12;
	float yborder = height/12;
	float eValue, scale = 0;
  for (it = state->data.begin(); it != state->data.end(); it++) {//find max value for scaling
		eValue = pow(it->second.real(),2)+pow(it->second.imag(),2);
    if (eValue > scale){
			scale = eValue;
		}
  }
	float rValue,iValue;//expectation value, real value, imaginary value
	//scaled values
	float barWidth = (width-2*xborder)/((float)state->dim);
	float sHeight = height -2*yborder;

  for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = pow(iValue,2) + pow(rValue,2);
			cr->set_source_rgb (1,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->fill();
	for (unsigned int i =0; i < state->dim; i++) {
		if(state->data.find(i)!= state->data.end()){
			rValue = state->data.find(i)->second.real();
			iValue = state->data.find(i)->second.imag();
			eValue = pow(iValue,2) + pow(rValue,2);
			cr->set_source_rgb (0,0,0);
			cr->rectangle (barWidth*i + xborder, sHeight*(1-eValue/scale)+yborder, barWidth, sHeight*eValue/scale);
		}
  }
	cr->stroke();
}
