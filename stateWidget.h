#include <gtkmm/drawingarea.h>
#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>

class StateWidget : public Gtk::DrawingArea {
public:
  StateWidget ();
	void set_state(State *state);
	void force_redraw ();
protected:
  virtual bool on_expose_event(GdkEventExpose* event);
private:
	void draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height);
	State *state;
};
