#include "stateView.h"
#include <cairomm/context.h>


stateView::stateView() {
  set_title("State Graph");
  set_border_width(0);
  add(m_vbox);
	draw.m_statusbar = & m_statusbar;
  draw.set_window(get_parent_window());
  draw.show();
	m_vbox.pack_end (m_statusbar, Gtk::PACK_SHRINK);
  m_vbox.pack_start (draw);
	m_vbox.show();
  show_all_children ();
}

void stateView::set_state(State *n_state){
	draw.set_state(n_state);
}

void stateView::redraw(){
	draw.force_redraw();
}

stateView::~stateView(){
}

