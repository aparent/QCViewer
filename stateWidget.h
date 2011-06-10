#include <gtkmm/drawingarea.h>
#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>
#include <string>

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
	
std::string draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height, float mousex, float mousey);
