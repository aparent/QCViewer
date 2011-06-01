#ifndef STATEVIEW__INCLUDED
#define STATEVIEW__INCLUDED
#include <gtkmm.h>
#include <state.h>
#include "stateWidget.h"
class stateView : public Gtk::Window
{
public:
  stateView ();
  stateView (State s);

	void set_state(State*);
  virtual ~stateView();
protected:
  Gtk::VBox m_vbox;
	StateWidget draw;
private: 
};
#endif
