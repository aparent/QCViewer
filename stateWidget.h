#include <gtkmm/drawingarea.h>
#include <cairo.h>
#include <cairomm/context.h>
#include <gtkmm.h>
#include <state.h>
#include <string>

const unsigned int numBuckets = 256;


class StateWidget : public Gtk::DrawingArea {
public:
  StateWidget (Gtk::Statusbar*);
	void set_state(State *state);
	void set_trace(index_t);
	void parse_state ();
	void parse_state_trace ();
	void reset ();
	enum DrawMode {STATEDRAW_EXPECTED, STATEDRAW_EXPECTED_TRACED, STATEDRAW_REAL, STATEDRAW_IMAG } drawmode;
protected:
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool onMotionEvent (GdkEventMotion* event);
private:
  double width, height;
	double xborder, yborder;
	double tickwidth;
	double barWidth, barHeight;
	double t_barWidth;

	void force_redraw ();
	State *state;
	Gtk::Statusbar* status;
	int mousex, mousey;
	bool draw_compressed;
	unsigned int num_draw; // number of buckets to draw
  index_t bucketID[numBuckets]; // XXX: need to be bitstrings
	std::complex<float_t> bucket[numBuckets];

	index_t trace;  // This is a bit string where 1s indicate parts to be included in the expectation
	float_type traced_bucket[numBuckets];// These hold the expectation value of a traced out state
	unsigned int num_draw_traced;
};

class StateViewWidget : public Gtk::VBox {
public:
  StateViewWidget (Gtk::Statusbar*);
	void reset ();
	void set_state (State*);

protected:
  void set_style ();

private:
  StateWidget sw;
	Gtk::HBox buttonbox;
	Gtk::VBox layoutbox;
	Gtk::RadioButton btn_expected, btn_real, btn_imag;
	Gtk::CheckButton btn_trace;
	Gtk::Statusbar* status;
};

std::string draw_state (Cairo::RefPtr<Cairo::Context> cr, State* state, float width ,float height, float mousex, float mousey);
