#include <gtkmm/drawingarea.h>
#include "drawState.h"

class StateWidget : public Gtk::DrawingArea {
public:
  StateWidget ();
	void set_state(State *state);
	void force_redraw ();
	Gtk::Statusbar *m_statusbar;
protected:
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool onMotionEvent (GdkEventMotion* event); 
private:
	State *state;
	int mousex, mousey;
};
